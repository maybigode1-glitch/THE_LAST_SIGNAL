#include "FlashlightBatteryMath.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

namespace
{
    void ExpectFloat(const char* name, float actual, float expected)
    {
        if (std::fabs(actual - expected) > 0.0001f)
        {
            std::cerr << name << ": expected " << expected << ", got " << actual << '\n';
            std::exit(EXIT_FAILURE);
        }
    }

    void ExpectRecharge(
        const char* name,
        float current,
        float charge,
        bool expectedAccepted,
        float expectedBattery
    )
    {
        float updated = current;
        const bool accepted = LastSignal::TryRechargeFlashlightBattery(current, charge, updated);
        if (accepted != expectedAccepted)
        {
            std::cerr << name << ": expected accepted=" << expectedAccepted
                      << ", got " << accepted << '\n';
            std::exit(EXIT_FAILURE);
        }
        ExpectFloat(name, updated, expectedBattery);
    }

    void ExpectAliasedRecharge(
        const char* name,
        float initialBattery,
        float charge,
        bool expectedAccepted,
        float expectedBattery
    )
    {
        float battery = initialBattery;
        const bool accepted = LastSignal::TryRechargeFlashlightBattery(battery, charge, battery);
        if (accepted != expectedAccepted)
        {
            std::cerr << name << ": expected accepted=" << expectedAccepted
                      << ", got " << accepted << '\n';
            std::exit(EXIT_FAILURE);
        }
        ExpectFloat(name, battery, expectedBattery);
    }
}

int main()
{
    using Limits = std::numeric_limits<float>;

    ExpectFloat("initial battery", LastSignal::ClampFlashlightBattery(100.0f), 100.0f);
    ExpectFloat("partial battery", LastSignal::ClampFlashlightBattery(42.5f), 42.5f);
    ExpectFloat("negative battery clamps to zero", LastSignal::ClampFlashlightBattery(-1.0f), 0.0f);
    ExpectFloat("NaN battery fails closed", LastSignal::ClampFlashlightBattery(Limits::quiet_NaN()), 0.0f);
    ExpectFloat("infinite battery fails closed", LastSignal::ClampFlashlightBattery(Limits::infinity()), 0.0f);

    ExpectFloat("ordinary drain", LastSignal::DrainFlashlightBattery(100.0f, 0.35f, 2.0f), 99.3f);
    ExpectFloat("drain reaches zero", LastSignal::DrainFlashlightBattery(10.0f, 5.0f, 2.0f), 0.0f);
    ExpectFloat("zero drain rate preserves charge", LastSignal::DrainFlashlightBattery(50.0f, 0.0f, 2.0f), 50.0f);
    ExpectFloat("negative drain rate is ignored", LastSignal::DrainFlashlightBattery(50.0f, -1.0f, 2.0f), 50.0f);
    ExpectFloat("NaN drain rate is ignored", LastSignal::DrainFlashlightBattery(50.0f, Limits::quiet_NaN(), 2.0f), 50.0f);
    ExpectFloat("negative delta is ignored", LastSignal::DrainFlashlightBattery(50.0f, 1.0f, -2.0f), 50.0f);
    ExpectFloat("infinite delta is ignored", LastSignal::DrainFlashlightBattery(50.0f, 1.0f, Limits::infinity()), 50.0f);
    ExpectFloat("large finite drain cannot overflow", LastSignal::DrainFlashlightBattery(100.0f, Limits::max(), Limits::max()), 0.0f);

    ExpectRecharge("ordinary recharge", 20.0f, 35.0f, true, 55.0f);
    ExpectRecharge("recharge clamps to full", 90.0f, 35.0f, true, 100.0f);
    ExpectRecharge("exact recharge to full", 65.0f, 35.0f, true, 100.0f);
    ExpectRecharge("full battery rejects recharge", 100.0f, 35.0f, false, 100.0f);
    ExpectRecharge("zero charge is rejected", 20.0f, 0.0f, false, 20.0f);
    ExpectRecharge("negative charge is rejected", 20.0f, -1.0f, false, 20.0f);
    ExpectRecharge("NaN charge is rejected", 20.0f, Limits::quiet_NaN(), false, 20.0f);
    ExpectRecharge("infinite charge is rejected", 20.0f, Limits::infinity(), false, 20.0f);
    ExpectRecharge("NaN current battery is rejected", Limits::quiet_NaN(), 35.0f, false, 0.0f);

    ExpectAliasedRecharge("aliased ordinary recharge", 20.0f, 35.0f, true, 55.0f);
    ExpectAliasedRecharge("aliased full battery rejects recharge", 100.0f, 35.0f, false, 100.0f);
    ExpectAliasedRecharge("aliased NaN battery fails closed", Limits::quiet_NaN(), 35.0f, false, 0.0f);

    std::cout << "Flashlight battery rules: 25 cases passed\n";
    return EXIT_SUCCESS;
}
