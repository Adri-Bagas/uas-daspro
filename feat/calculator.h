#ifndef CALCULATOR_H
#define CALCULATOR_H

/**
 * RECURSIVE FUNCTION
 * Calculates future savings balance month by month.
 * * @param current_month     Counter (starts at 0 or 1)
 * @param total_months      Target duration in months
 * @param current_balance   Accumulated money so far
 * @param monthly_deposit   How much user adds every month
 * @param monthly_rate      Interest rate per month (e.g., 0.004 for 5% annual)
 * * @return The final balance after total_months.
 */
double calculate_savings_recursive(int current_month, int total_months, double current_balance, double monthly_deposit, double monthly_rate, double monthly_admin_fee);

int calculate_debt_payoff_recursive(double current_balance, double monthly_payment, double monthly_rate, int current_month, double *total_interest_paid);

/**
 * Menu handler to prompt user for input and show the result.
 */
void show_calculator_menu();

#endif