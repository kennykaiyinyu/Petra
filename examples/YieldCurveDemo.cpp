#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <string>
#include "GreekCore/Rates/YieldCurve.h"
#include "GreekCore/Time/Date.h"
#include "GreekCore/Time/Calendar.h"
#include "GreekCore/Time/NYSECalendar.h"
#include <algorithm>

using namespace GreekCore;
using namespace GreekCore::Time;

int main() {
    std::cout << "--- Yield Curve Bootstrapping Demo ---\n";

    // Representative USD SOFR Curve Data (Illustrative Market Data)
    // Reference Date: Feb 14, 2026
    Date today = make_date(2026, 2, 14);

    // Use a calendar for business day adjustments (Modified Following)
    NYSECalendar cal;

    // Adjust Reference Date if necessary (Previous Business Day)
    Date refDate = cal.is_business_day(today) ? today : 
                   [&](){ 
                       Date d = today; 
                       while(!cal.is_business_day(d)) d -= std::chrono::days(1); 
                       return d; 
                   }();

    std::chrono::year_month_day refYMD{refDate};
    std::cout << "Reference Date: " << static_cast<int>(refYMD.year()) << "-" 
              << static_cast<unsigned>(refYMD.month()) << "-" 
              << static_cast<unsigned>(refYMD.day()) << "\n";

    // Helper for adding business days (for O/N, T/N, Spot)
    auto add_business_days = [&](Date d, int n) {
        Date res = d;
        int step = (n > 0) ? 1 : -1;
        int count = std::abs(n);
        for(int i=0; i<count; ++i) {
            res += std::chrono::days(step);
            while(!cal.is_business_day(res)) res += std::chrono::days(step);
        }
        return res;
    };

    auto add_months = [&](Date d, int n) {
        using namespace std::chrono;
        year_month_day ymd{d};
        Date unadjusted{ymd + months(n)};
        return adjust(unadjusted, BusinessDayConvention::ModifiedFollowing, cal);
    };
    
    auto add_years = [&](Date d, int n) {
        using namespace std::chrono;
        year_month_day ymd{d};
        Date unadjusted{ymd + years(n)};
        return adjust(unadjusted, BusinessDayConvention::ModifiedFollowing, cal);
    };

    // Define Spot Date (T+2)
    Date spotDate = add_business_days(refDate, 2);

    struct DemoInstrument {
        std::string label;
        CurveInput input;
    };

    std::vector<DemoInstrument> instruments;

    // 1. Short End with Real Market Rates
    // O/N (Overnight SOFR): ~3.65%
    instruments.push_back({"O/N", {InstrumentType::Deposit, 0.0365, add_business_days(refDate, 1), refDate, 0}});
    
    // T/N (Tomorrow/Next): ~3.64%
    // Note: Start Date is Tomorrow, Maturity is Spot (T+2)
    Date tomDate = add_business_days(refDate, 1);
    instruments.push_back({"T/N", {InstrumentType::Deposit, 0.0364, spotDate, tomDate, 0}});

    // Spot Starting Deposits (Treasury Equivalents for simplicity)
    // 1M: 3.72% (starts from Ref Date)
    instruments.push_back({"1M", {InstrumentType::Deposit, 0.0372, add_months(refDate, 1), refDate, 0}});
    // 3M: 3.68% (starts from Ref Date)
    instruments.push_back({"3M", {InstrumentType::Deposit, 0.0368, add_months(refDate, 3), refDate, 0}});

    // 2. FRAs
    // 3x6 FRA: ~3.50% (Midpoint of 3.45-3.55)
    Date fra3m = add_months(spotDate, 3);
    Date fra6m = add_months(spotDate, 6);
    instruments.push_back({"3x6 FRA", {InstrumentType::FRA, 0.0350, fra6m, fra3m, 0}});

    // 6x9 FRA: ~3.40% (Midpoint of 3.35-3.45)
    Date fra9m = add_months(spotDate, 9);
    instruments.push_back({"6x9 FRA", {InstrumentType::FRA, 0.0340, fra9m, fra6m, 0}});

    // 3. SOFR Swaps (Calibrated to match US Treasury Yields for this demo)
    // 1Y Swap: ~3.42% (Interpolated)
    instruments.push_back({"1Y Swap", {InstrumentType::Swap, 0.0342, add_years(spotDate, 1), spotDate, 1}}); // Money Market / Swap usually annual
    // 2Y Swap: 3.40% (Matches 2Y T-Note - Annual Swap Rate act/360)
    instruments.push_back({"2Y Swap", {InstrumentType::Swap, 0.0340, add_years(spotDate, 2), spotDate, 1}});
    // 5Y Swap: ~3.65% (Interpolated - Annual Swap Rate act/360)
    instruments.push_back({"5Y Swap", {InstrumentType::Swap, 0.0365, add_years(spotDate, 5), spotDate, 1}});
    // 10Y Swap: 4.04% (Matches 10Y T-Note - Annual Swap Rate act/360)
    instruments.push_back({"10Y Swap", {InstrumentType::Swap, 0.0404, add_years(spotDate, 10), spotDate, 1}});
    // 30Y Swap: 4.73% (Matches 30Y T-Bond - Annual Swap Rate act/360)
    instruments.push_back({"30Y Swap", {InstrumentType::Swap, 0.0473, add_years(spotDate, 30), spotDate, 1}});

    // Extract CurveInputs for the engine
    std::vector<CurveInput> curve_inputs;
    curve_inputs.reserve(instruments.size());
    for(const auto& inst : instruments) {
        curve_inputs.push_back(inst.input);
    }

    // Print Inputs
    std::cout << "\n--- Market Inputs (Benchmarks) ---\n";
    std::cout << std::left << std::setw(15) << "Instrument"
              << std::setw(10) << "Type" 
              << std::setw(12) << "Rate" 
              << std::setw(15) << "Start Date"
              << std::setw(15) << "Maturity Date" << "\n";
    
    for (const auto& instr : instruments) {
        std::string typeStr = (instr.input.type == InstrumentType::Deposit) ? "Cash" : 
                              (instr.input.type == InstrumentType::FRA) ? "FRA" : "Swap";
        
        std::chrono::year_month_day ymd_mat{instr.input.maturity_date};
        std::chrono::year_month_day ymd_start{instr.input.start_date};

        std::cout << std::left << std::setw(15) << instr.label
                  << std::setw(10) << typeStr 
                  << std::fixed << std::setprecision(4) << (instr.input.rate * 100.0) << "%" 
                  << std::setw(5) << " "
                  << static_cast<int>(ymd_start.year()) << "-"  
                  << std::right << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd_start.month()) << "-" 
                  << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd_start.day()) << std::left << std::setfill(' ')
                  << std::setw(5) << " "
                  << static_cast<int>(ymd_mat.year()) << "-" 
                  << std::right << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd_mat.month()) << "-" 
                  << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd_mat.day()) << std::left << std::setfill(' ') << "\n";
    }

    // Interactive Selection
    std::cout << "\nSelect Day Count Convention:\n";
    std::cout << "1. Actual/365 (Fixed) [Default]\n";
    std::cout << "2. Actual/360\n";
    std::cout << "3. 30/360\n";
    std::cout << "4. Actual/Actual (ISDA)\n";
    std::cout << "5. Run All (Comparison)\n";
    std::cout << "Choice: ";

    int choice = 0;
    std::cin >> choice;

    auto run_bootstrapping = [&](auto dc_instance, const std::string& name) {
        using DC = decltype(dc_instance);
        std::cout << "\n--- Bootstrapped Curve (" << name << ") ---\n";
        std::cout << std::left << std::setw(20) << "Instrument & Tenor"
                  << std::setw(15) << "Start Date" 
                  << std::setw(15) << "End Date" 
                  << std::setw(15) << "Input Freq"
                  << std::setw(28) << "Implied Discount Factor" 
                  << std::setw(20) << "Zero Rate (Cont.)" << "\n";

        YieldCurve<DC> curve(refDate, curve_inputs);

        for (const auto& instr : instruments) {
            double df = curve.getDiscountFactor(instr.input.maturity_date);
            double zr = curve.getZeroRate(instr.input.maturity_date);
            
            std::chrono::year_month_day ymd_start{instr.input.start_date};
            std::chrono::year_month_day ymd_end{instr.input.maturity_date};

            std::string freqStr = (instr.input.type == InstrumentType::Swap) 
                                  ? (instr.input.frequency == 1 ? "Annual" : 
                                     instr.input.frequency == 2 ? "Semi-Annual" : "Quarterly")
                                  : "Simple/None";

            if (instr.input.type == InstrumentType::FRA) freqStr = "Simple";
            if (instr.input.type == InstrumentType::Deposit) freqStr = "Simple";

            std::cout << std::left << std::setw(20) << instr.label
                      << static_cast<int>(ymd_start.year()) << "-" 
                      << std::right << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd_start.month()) << "-" 
                      << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd_start.day()) << std::left << std::setfill(' ')
                      << std::setw(5) << " " 
                      << static_cast<int>(ymd_end.year()) << "-" 
                      << std::right << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd_end.month()) << "-" 
                      << std::setw(2) << std::setfill('0') << static_cast<unsigned>(ymd_end.day()) << std::left << std::setfill(' ')
                      << std::setw(5) << " "
                      << std::setw(15) << freqStr
                      << std::setw(28) << std::fixed << std::setprecision(8) << df 
                      << std::setw(20) << std::setprecision(4) << (zr * 100.0) << "%" << "\n";
        }
    };

    if (choice == 1 || choice == 5) run_bootstrapping(Act365DayCounter{}, "Actual/365");
    if (choice == 2 || choice == 5) run_bootstrapping(Act360DayCounter{}, "Actual/360");
    if (choice == 3 || choice == 5) run_bootstrapping(Thirty360DayCounter{}, "30/360");
    if (choice == 4 || choice == 5) run_bootstrapping(ActActDayCounter{}, "Actual/Actual");

    return 0;
}