"""Keep the Unreal map builder and editor QA actor contract in sync."""
import ast
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def assigned_literal(tree, name):
    for node in tree.body:
        if isinstance(node, ast.Assign) and any(
            isinstance(target, ast.Name) and target.id == name
            for target in node.targets
        ):
            return ast.literal_eval(node.value)
    raise AssertionError(f"Assignment not found: {name}")


def loaded_class_paths(tree):
    result = {}
    for node in ast.walk(tree):
        if not isinstance(node, ast.Assign) or not isinstance(node.value, ast.Call):
            continue
        function = node.value.func
        if not isinstance(function, ast.Attribute) or function.attr != "load_class":
            continue
        if len(node.value.args) < 2:
            continue
        try:
            path = ast.literal_eval(node.value.args[1])
        except (ValueError, TypeError):
            continue
        if isinstance(path, str):
            for target in node.targets:
                if isinstance(target, ast.Name):
                    result[target.id] = path.rsplit(".", 1)[-1]
    return result


def class_name_for(expression, loaded_classes):
    if isinstance(expression, ast.Name):
        return loaded_classes.get(expression.id)
    if isinstance(expression, ast.Attribute):
        return expression.attr
    return None


class RelayStationBuilderContractTests(unittest.TestCase):
    def test_builder_labels_and_classes_match_editor_qa_requirements(self):
        qa_path = ROOT / "Scripts" / "qa_relay_station.py"
        builder_path = ROOT / "Scripts" / "create_relay_station_map.py"
        qa_tree = ast.parse(qa_path.read_text(encoding="utf-8"), filename=str(qa_path))
        builder_tree = ast.parse(
            builder_path.read_text(encoding="utf-8"), filename=str(builder_path)
        )

        qa_prefix = assigned_literal(qa_tree, "GENERATED_PREFIX")
        builder_prefix = assigned_literal(builder_tree, "GENERATED_PREFIX")
        self.assertEqual(builder_prefix, qa_prefix)

        required = assigned_literal(qa_tree, "REQUIRED_GENERATED_ACTOR_CLASSES")
        self.assertTrue(required)
        loaded_classes = loaded_class_paths(builder_tree)
        label_positions = {"spawn_actor": (3, 0), "spawn_interactable": (2, 0)}
        actual = {}

        for node in ast.walk(builder_tree):
            if not isinstance(node, ast.Call) or not isinstance(node.func, ast.Name):
                continue
            call_name = node.func.id
            if call_name not in label_positions:
                continue
            label_index, class_index = label_positions[call_name]
            if len(node.args) <= max(label_index, class_index):
                continue
            try:
                label = ast.literal_eval(node.args[label_index])
            except (ValueError, TypeError):
                continue
            if not isinstance(label, str):
                continue
            full_label = builder_prefix + label
            actual.setdefault(full_label, []).append(
                class_name_for(node.args[class_index], loaded_classes)
            )

        self.assertEqual(set(actual), set(required))
        for label, expected_class in required.items():
            with self.subTest(label=label):
                self.assertEqual(actual[label], [expected_class])


if __name__ == "__main__":
    unittest.main()
