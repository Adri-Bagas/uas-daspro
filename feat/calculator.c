#include <stdio.h>
#include <stdlib.h>
#include "calculator.h"
#include "utils.h"

double calculate_savings_recursive(int current_month, int total_months, double current_balance, double monthly_deposit, double monthly_rate, double monthly_admin_fee) {
    // 1. BASE CASE: If we reached the target month, stop and return result.
    if (current_month >= total_months) {
        return current_balance;
    }


    double balance_after_deposit = current_balance + monthly_deposit;
    double interest_earned = balance_after_deposit * monthly_rate;
    

    double new_balance = balance_after_deposit + interest_earned - monthly_admin_fee;

    // Print progress setiap tahun 
    if ((current_month + 1) % 12 == 0) {
        printf("   End of Year %d: %.2f\n", (current_month + 1) / 12, new_balance);
    }

    
    return calculate_savings_recursive(current_month + 1, total_months, new_balance, monthly_deposit, monthly_rate, monthly_admin_fee);
}

int calculate_debt_payoff_recursive(double current_balance, double monthly_payment, double monthly_rate, int current_month, double *total_interest_paid) {

    if (current_balance <= 0) {
        return current_month;
    }


    if (current_month > 600) { // max 600 bulan
        return -1;
    }


    double interest = current_balance * monthly_rate;
    double principal_paid = monthly_payment - interest;
    double new_balance = current_balance - principal_paid;
    
    *total_interest_paid += interest;

    // Print progress setiap tahun 
    if ((current_month + 1) % 12 == 0) {
        printf("   End of Year %d: Remaining Debt %.2f\n", (current_month + 1) / 12, new_balance > 0 ? new_balance : 0);
    }

    return calculate_debt_payoff_recursive(new_balance, monthly_payment, monthly_rate, current_month + 1, total_interest_paid);
}

void show_calculator_menu() {
    char buffer[100];
    int choice;

    clear_screen();
    printf("FINANCIAL CALCULATORS\n");
    printf("------------------------------\n");
    printf("1. Savings Projection (Future Value)\n");
    printf("2. Debt Payoff Estimator (Time to Freedom)\n");
    printf("------------------------------\n");
    printf("Select Calculator (1-2): ");
    
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
    choice = atoi(buffer);

    if (choice == 1) {
        clear_screen();
        
        double initial_amount, monthly_deposit, annual_interest, monthly_admin_fee;
        int years;

        printf("\n--- SAVINGS PROJECTION ---\n");
        printf("Current Savings Amount: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
        initial_amount = atof(buffer);

        printf("Monthly Contribution: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
        monthly_deposit = atof(buffer);

        printf("Annual Interest Rate (%): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
        annual_interest = atof(buffer);

        printf("Monthly Admin Fee: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
        monthly_admin_fee = atof(buffer);

        printf("Duration (Years): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
        years = atoi(buffer);

        double monthly_rate = (annual_interest / 100.0) / 12.0;
        int total_months = years * 12;

        printf("\n[Calculating Projection...]\n");
        double final_amount = calculate_savings_recursive(0, total_months, initial_amount, monthly_deposit, monthly_rate, monthly_admin_fee);

        printf("------------------------------\n");
        printf("RESULT:\n");
        printf("In %d years, you will have: %.2f\n", years, final_amount);

    } else if (choice == 2) {
        clear_screen();
        
        double total_debt, monthly_payment, annual_interest;
        
        printf("\n--- DEBT PAYOFF ESTIMATOR ---\n");
        printf("Total Debt Amount: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
        total_debt = atof(buffer);

        printf("Annual Interest Rate (%): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
        annual_interest = atof(buffer);

        printf("Monthly Payment Budget: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) return;
        monthly_payment = atof(buffer);

        // Validation: Payment must cover interest
        double monthly_rate = (annual_interest / 100.0) / 12.0;
        double min_interest = total_debt * monthly_rate;

        if (monthly_payment <= min_interest) {
            fprintf(stderr, "Payment too low! You cannot cover the monthly interest.");
            printf("   Min required: %.2f\n", min_interest + 1);
        } else {
            printf("\n[Calculating Payoff Timeline...]\n");
            double total_interest = 0;
            int months = calculate_debt_payoff_recursive(total_debt, monthly_payment, monthly_rate, 0, &total_interest);

            printf("------------------------------\n");
            printf("RESULT:\n");
            if (months == -1) {
                printf("It will take over 50 years. Please increase your payment.\n");
            } else {
                int years = months / 12;
                int rem_months = months % 12;
                printf("Time to Debt Free: %d Years, %d Months\n", years, rem_months);
                printf("Total Interest Paid: %.2f\n", total_interest);
                printf("Total Amount Paid: %.2f\n", total_debt + total_interest);
            }
        }
    } else {
        fprintf(stderr, "Invalid selection.");
    }
    
    printf("\nPress Enter to return...");
    getchar();
}