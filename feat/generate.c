#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generate.h"

// bagian header dari html
#define HEADER "<!DOCTYPE html>\n"                                                                                                                        \
               "<html lang=\"en\">\n"                                                                                                                     \
               "<head>\n"                                                                                                                                 \
               "    <meta charset=\"UTF-8\">\n"                                                                                                           \
               "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"                                                         \
               "    <title>Financial Report - %s</title>\n"                                                                                               \
               "    \n"                                                                                                                                   \
               "    <!-- Fonts: Inter -->\n"                                                                                                              \
               "    <link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n"                                                                    \
               "    <link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>\n"                                                           \
               "    <link href=\"https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&display=swap\" rel=\"stylesheet\">\n"                \
               "    \n"                                                                                                                                   \
               "    <!-- Tailwind CSS (CDN) -->\n"                                                                                                        \
               "    <script src=\"https://cdn.tailwindcss.com\"></script>\n"                                                                              \
               "    \n"                                                                                                                                   \
               "    <!-- Chart.js (CDN) -->\n"                                                                                                            \
               "    <script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n"                                                                    \
               "\n"                                                                                                                                       \
               "    <script>\n"                                                                                                                           \
               "        tailwind.config = {\n"                                                                                                            \
               "            theme: {\n"                                                                                                                   \
               "                extend: {\n"                                                                                                              \
               "                    fontFamily: {\n"                                                                                                      \
               "                        sans: ['Inter', 'sans-serif'],\n"                                                                                 \
               "                    },\n"                                                                                                                 \
               "                    colors: {\n"                                                                                                          \
               "                        primary: '#4F46E5', // Indigo 600\n"                                                                              \
               "                        secondary: '#10B981', // Emerald 500\n"                                                                           \
               "                        danger: '#EF4444', // Red 500\n"                                                                                  \
               "                        dark: '#1E293B', // Slate 800\n"                                                                                  \
               "                    }\n"                                                                                                                  \
               "                }\n"                                                                                                                      \
               "            }\n"                                                                                                                          \
               "        }\n"                                                                                                                              \
               "    </script>\n"                                                                                                                          \
               "    \n"                                                                                                                                   \
               "    <style>\n"                                                                                                                            \
               "        body { font-family: 'Inter', sans-serif; background-color: #F1F5F9; }\n"                                                          \
               "        .card { background: white; border-radius: 12px; box-shadow: 0 1px 3px 0 rgba(0, 0, 0, 0.1), 0 1px 2px 0 rgba(0, 0, 0, 0.06); }\n" \
               "        @media print {\n"                                                                                                                 \
               "            .no-print { display: none; }\n"                                                                                               \
               "            body { background-color: white; }\n"                                                                                          \
               "            .card { box-shadow: none; border: 1px solid #e2e8f0; }\n"                                                                     \
               "        }\n"                                                                                                                              \
               "    </style>\n"                                                                                                                           \
               "</head>\n"

// Main HTML Body tempat injek data
#define BODY "<body class=\"text-slate-800 antialiased\"> \n"                                                                                                                                                                                                                                                                         \
             "    <!-- Top Navigation / Header -->\n"                                                                                                                                                                                                                                                                                 \
             "    <nav class=\"bg-white border-b border-slate-200 sticky top-0 z-50\">\n"                                                                                                                                                                                                                                             \
             "        <div class=\"max-w-7xl mx-auto px-4 sm:px-6 lg:px-8\">\n"                                                                                                                                                                                                                                                       \
             "            <div class=\"flex justify-between h-16\">\n"                                                                                                                                                                                                                                                                \
             "                <div class=\"flex items-center\">\n"                                                                                                                                                                                                                                                                    \
             "                    <!-- Logo / Title -->\n"                                                                                                                                                                                                                                                                            \
             "                    <div class=\"flex-shrink-0 flex items-center gap-3\">\n"                                                                                                                                                                                                                                            \
             "                        <div class=\"w-8 h-8 bg-primary rounded-lg flex items-center justify-center text-white font-bold\">\n"                                                                                                                                                                                          \
             "                            F\n"                                                                                                                                                                                                                                                                                        \
             "                        </div>\n"                                                                                                                                                                                                                                                                                       \
             "                        <h1 class=\"text-xl font-bold tracking-tight text-slate-900\">Finance<span class=\"text-primary\">Manager</span></h1>\n"                                                                                                                                                                        \
             "                    </div>\n"                                                                                                                                                                                                                                                                                           \
             "                    <div class=\"hidden md:ml-6 md:flex md:space-x-8\">\n"                                                                                                                                                                                                                                              \
             "                        <span class=\"border-primary text-slate-900 inline-flex items-center px-1 pt-1 border-b-2 text-sm font-medium\">\n"                                                                                                                                                                             \
             "                            Monthly Report\n"                                                                                                                                                                                                                                                                           \
             "                        </span>\n"                                                                                                                                                                                                                                                                                      \
             "                    </div>\n"                                                                                                                                                                                                                                                                                           \
             "                </div>\n"                                                                                                                                                                                                                                                                                               \
             "                <div class=\"flex items-center gap-4\">\n"                                                                                                                                                                                                                                                              \
             "                    <span id=\"period-badge\" class=\"px-3 py-1 rounded-full text-xs font-semibold bg-indigo-50 text-indigo-700 border border-indigo-100\">\n"                                                                                                                                                          \
             "                        Loading...\n"                                                                                                                                                                                                                                                                                   \
             "                    </span>\n"                                                                                                                                                                                                                                                                                          \
             "                    <button onclick=\"window.print()\" class=\"no-print inline-flex items-center px-4 py-2 border border-transparent text-sm font-medium rounded-md shadow-sm text-white bg-slate-900 hover:bg-slate-800 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-indigo-500 transition-all\">\n" \
             "                        Print / Save PDF\n"                                                                                                                                                                                                                                                                             \
             "                    </button>\n"                                                                                                                                                                                                                                                                                        \
             "                </div>\n"                                                                                                                                                                                                                                                                                               \
             "            </div>\n"                                                                                                                                                                                                                                                                                                   \
             "        </div>\n"                                                                                                                                                                                                                                                                                                       \
             "    </nav>\n"                                                                                                                                                                                                                                                                                                           \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "    <!-- Main Content -->\n"                                                                                                                                                                                                                                                                                            \
             "    <main class=\"max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-8\">\n"                                                                                                                                                                                                                                                     \
             "        \n"                                                                                                                                                                                                                                                                                                             \
             "        <!-- Header Text -->\n"                                                                                                                                                                                                                                                                                         \
             "        <div class=\"mb-8\">\n"                                                                                                                                                                                                                                                                                         \
             "            <h2 class=\"text-2xl font-bold leading-7 text-slate-900 sm:text-3xl sm:truncate\">\n"                                                                                                                                                                                                                       \
             "                Financial Overview\n"                                                                                                                                                                                                                                                                                   \
             "            </h2>\n"                                                                                                                                                                                                                                                                                                    \
             "            <p class=\"mt-1 text-sm text-slate-500\">\n"                                                                                                                                                                                                                                                                \
             "                Detailed breakdown of income, expenses, and savings performance for <span id=\"period-text\" class=\"font-medium\">...</span>.\n"                                                                                                                                                                       \
             "            </p>\n"                                                                                                                                                                                                                                                                                                     \
             "        </div>\n"                                                                                                                                                                                                                                                                                                       \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "        <!-- Summary Cards -->\n"                                                                                                                                                                                                                                                                                       \
             "        <div class=\"grid grid-cols-1 gap-5 sm:grid-cols-3 mb-8\">\n"                                                                                                                                                                                                                                                   \
             "            <!-- Income Card -->\n"                                                                                                                                                                                                                                                                                     \
             "            <div class=\"card p-6 border-l-4 border-emerald-500\">\n"                                                                                                                                                                                                                                                   \
             "                <div class=\"flex items-center\">\n"                                                                                                                                                                                                                                                                    \
             "                    <div class=\"flex-shrink-0 bg-emerald-100 rounded-md p-3\">\n"                                                                                                                                                                                                                                      \
             "                        <svg class=\"h-6 w-6 text-emerald-600\" fill=\"none\" stroke=\"currentColor\" viewBox=\"0 0 24 24\">\n"                                                                                                                                                                                         \
             "                            <path stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"2\" d=\"M13 7h8m0 0v8m0-8l-8 8-4-4-6 6\" />\n"                                                                                                                                                                      \
             "                        </svg>\n"                                                                                                                                                                                                                                                                                       \
             "                    </div>\n"                                                                                                                                                                                                                                                                                           \
             "                    <div class=\"ml-5 w-0 flex-1\">\n"                                                                                                                                                                                                                                                                  \
             "                        <dl>\n"                                                                                                                                                                                                                                                                                         \
             "                            <dt class=\"text-sm font-medium text-slate-500 truncate\">Total Income</dt>\n"                                                                                                                                                                                                              \
             "                            <dd class=\"flex items-baseline\">\n"                                                                                                                                                                                                                                                       \
             "                                <div id=\"summary-income\" class=\"text-2xl font-semibold text-slate-900\">...</div>\n"                                                                                                                                                                                                 \
             "                            </dd>\n"                                                                                                                                                                                                                                                                                    \
             "                        </dl>\n"                                                                                                                                                                                                                                                                                        \
             "                    </div>\n"                                                                                                                                                                                                                                                                                           \
             "                </div>\n"                                                                                                                                                                                                                                                                                               \
             "            </div>\n"                                                                                                                                                                                                                                                                                                   \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "            <!-- Expense Card -->\n"                                                                                                                                                                                                                                                                                    \
             "            <div class=\"card p-6 border-l-4 border-red-500\">\n"                                                                                                                                                                                                                                                       \
             "                <div class=\"flex items-center\">\n"                                                                                                                                                                                                                                                                    \
             "                    <div class=\"flex-shrink-0 bg-red-100 rounded-md p-3\">\n"                                                                                                                                                                                                                                          \
             "                        <svg class=\"h-6 w-6 text-red-600\" fill=\"none\" stroke=\"currentColor\" viewBox=\"0 0 24 24\">\n"                                                                                                                                                                                             \
             "                            <path stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"2\" d=\"M13 17h8m0 0V9m0 8l-8-8-4 4-6-6\" />\n"                                                                                                                                                                     \
             "                        </svg>\n"                                                                                                                                                                                                                                                                                       \
             "                    </div>\n"                                                                                                                                                                                                                                                                                           \
             "                    <div class=\"ml-5 w-0 flex-1\">\n"                                                                                                                                                                                                                                                                  \
             "                        <dl>\n"                                                                                                                                                                                                                                                                                         \
             "                            <dt class=\"text-sm font-medium text-slate-500 truncate\">Total Expense</dt>\n"                                                                                                                                                                                                             \
             "                            <dd class=\"flex items-baseline\">\n"                                                                                                                                                                                                                                                       \
             "                                <div id=\"summary-expense\" class=\"text-2xl font-semibold text-slate-900\">...</div>\n"                                                                                                                                                                                                \
             "                            </dd>\n"                                                                                                                                                                                                                                                                                    \
             "                        </dl>\n"                                                                                                                                                                                                                                                                                        \
             "                    </div>\n"                                                                                                                                                                                                                                                                                           \
             "                </div>\n"                                                                                                                                                                                                                                                                                               \
             "            </div>\n"                                                                                                                                                                                                                                                                                                   \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "            <!-- Savings Card -->\n"                                                                                                                                                                                                                                                                                    \
             "            <div class=\"card p-6 border-l-4 border-indigo-500\">\n"                                                                                                                                                                                                                                                    \
             "                <div class=\"flex items-center\">\n"                                                                                                                                                                                                                                                                    \
             "                    <div class=\"flex-shrink-0 bg-indigo-100 rounded-md p-3\">\n"                                                                                                                                                                                                                                       \
             "                        <svg class=\"h-6 w-6 text-indigo-600\" fill=\"none\" stroke=\"currentColor\" viewBox=\"0 0 24 24\">\n"                                                                                                                                                                                          \
             "                            <path stroke-linecap=\"round\" stroke-linejoin=\"round\" stroke-width=\"2\" d=\"M12 8c-1.657 0-3 .895-3 2s1.343 2 3 2 3 .895 3 2-1.343 2-3 2m0-8c1.11 0 2.08.402 2.599 1M12 8V7m0 1v8m0 0v1m0-1c-1.11 0-2.08-.402-2.599-1M21 12a9 9 0 11-18 0 9 9 0 0118 0z\" />\n"                         \
             "                        </svg>\n"                                                                                                                                                                                                                                                                                       \
             "                    </div>\n"                                                                                                                                                                                                                                                                                           \
             "                    <div class=\"ml-5 w-0 flex-1\">\n"                                                                                                                                                                                                                                                                  \
             "                        <dl>\n"                                                                                                                                                                                                                                                                                         \
             "                            <dt class=\"text-sm font-medium text-slate-500 truncate\">Total Savings</dt>\n"                                                                                                                                                                                                             \
             "                            <dd class=\"flex items-baseline\">\n"                                                                                                                                                                                                                                                       \
             "                                <div id=\"summary-savings\" class=\"text-2xl font-semibold text-slate-900\">...</div>\n"                                                                                                                                                                                                \
             "                            </dd>\n"                                                                                                                                                                                                                                                                                    \
             "                        </dl>\n"                                                                                                                                                                                                                                                                                        \
             "                    </div>\n"                                                                                                                                                                                                                                                                                           \
             "                </div>\n"                                                                                                                                                                                                                                                                                               \
             "            </div>\n"                                                                                                                                                                                                                                                                                                   \
             "        </div>\n"                                                                                                                                                                                                                                                                                                       \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "        <!-- Main Chart: Daily Trend -->\n"                                                                                                                                                                                                                                                                             \
             "        <div class=\"card p-6 mb-8\">\n"                                                                                                                                                                                                                                                                                \
             "            <h3 class=\"text-lg leading-6 font-medium text-slate-900 mb-4\">Daily Spending Trend</h3>\n"                                                                                                                                                                                                                \
             "            <div class=\"relative h-80 w-full\">\n"                                                                                                                                                                                                                                                                     \
             "                <canvas id=\"trendChart\"></canvas>\n"                                                                                                                                                                                                                                                                  \
             "            </div>\n"                                                                                                                                                                                                                                                                                                   \
             "        </div>\n"                                                                                                                                                                                                                                                                                                       \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "        <!-- Bottom Grid: Allocation & Top Categories -->\n"                                                                                                                                                                                                                                                            \
             "        <div class=\"grid grid-cols-1 lg:grid-cols-2 gap-8\">\n"                                                                                                                                                                                                                                                        \
             "            \n"                                                                                                                                                                                                                                                                                                         \
             "            <!-- Allocation Chart -->\n"                                                                                                                                                                                                                                                                                \
             "            <div class=\"card p-6\">\n"                                                                                                                                                                                                                                                                                 \
             "                <h3 class=\"text-lg leading-6 font-medium text-slate-900 mb-4\">Budget Allocation (50/30/20 Rule)</h3>\n"                                                                                                                                                                                               \
             "                <div class=\"relative h-64 w-full flex justify-center\">\n"                                                                                                                                                                                                                                             \
             "                    <canvas id=\"allocationChart\"></canvas>\n"                                                                                                                                                                                                                                                         \
             "                </div>\n"                                                                                                                                                                                                                                                                                               \
             "                <div class=\"mt-4 grid grid-cols-3 text-center divide-x divide-slate-200\">\n"                                                                                                                                                                                                                          \
             "                    <div>\n"                                                                                                                                                                                                                                                                                            \
             "                        <p class=\"text-xs text-slate-500 uppercase\">Needs</p>\n"                                                                                                                                                                                                                                      \
             "                        <p id=\"alloc-needs\" class=\"text-sm font-bold text-slate-800\">...</p>\n"                                                                                                                                                                                                                     \
             "                    </div>\n"                                                                                                                                                                                                                                                                                           \
             "                    <div>\n"                                                                                                                                                                                                                                                                                            \
             "                        <p class=\"text-xs text-slate-500 uppercase\">Wants</p>\n"                                                                                                                                                                                                                                      \
             "                        <p id=\"alloc-wants\" class=\"text-sm font-bold text-slate-800\">...</p>\n"                                                                                                                                                                                                                     \
             "                    </div>\n"                                                                                                                                                                                                                                                                                           \
             "                    <div>\n"                                                                                                                                                                                                                                                                                            \
             "                        <p class=\"text-xs text-slate-500 uppercase\">Savings</p>\n"                                                                                                                                                                                                                                    \
             "                        <p id=\"alloc-savings\" class=\"text-sm font-bold text-slate-800\">...</p>\n"                                                                                                                                                                                                                   \
             "                    </div>\n"                                                                                                                                                                                                                                                                                           \
             "                </div>\n"                                                                                                                                                                                                                                                                                               \
             "            </div>\n"                                                                                                                                                                                                                                                                                                   \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "            <!-- Top Categories Chart -->\n"                                                                                                                                                                                                                                                                            \
             "            <div class=\"card p-6\">\n"                                                                                                                                                                                                                                                                                 \
             "                <h3 class=\"text-lg leading-6 font-medium text-slate-900 mb-4\">Top Spending Categories</h3>\n"                                                                                                                                                                                                         \
             "                <div class=\"relative h-72 w-full\">\n"                                                                                                                                                                                                                                                                 \
             "                    <canvas id=\"categoryChart\"></canvas>\n"                                                                                                                                                                                                                                                           \
             "                </div>\n"                                                                                                                                                                                                                                                                                               \
             "            </div>\n"                                                                                                                                                                                                                                                                                                   \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "        </div>\n"                                                                                                                                                                                                                                                                                                       \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "    </main>\n"                                                                                                                                                                                                                                                                                                          \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "    <!-- Footer -->\n"                                                                                                                                                                                                                                                                                                  \
             "    <footer class=\"bg-white border-t border-slate-200 mt-12\">\n"                                                                                                                                                                                                                                                      \
             "        <div class=\"max-w-7xl mx-auto py-6 px-4 sm:px-6 md:flex md:items-center md:justify-between lg:px-8\">\n"                                                                                                                                                                                                       \
             "            <div class=\"mt-8 md:mt-0 md:order-1\">\n"                                                                                                                                                                                                                                                                  \
             "                <p class=\"text-center text-sm text-slate-400\">\n"                                                                                                                                                                                                                                                     \
             "                    &copy; 2025 Finance Manager App (Group 5). Generated automatically.\n"                                                                                                                                                                                                                              \
             "                </p>\n"                                                                                                                                                                                                                                                                                                 \
             "            </div>\n"                                                                                                                                                                                                                                                                                                   \
             "        </div>\n"                                                                                                                                                                                                                                                                                                       \
             "    </footer>\n"                                                                                                                                                                                                                                                                                                        \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "    <!-- LOGIC SCRIPT -->\n"                                                                                                                                                                                                                                                                                            \
             "    <script>\n"                                                                                                                                                                                                                                                                                                         \
             "       //aku muak concat di c wkwkwkwkwkwkw\n"                                                                                                                                                                                                                                                                          \
             "       const allocation_array = [%s];\n"                                                                                                                                                                                                                                                                                \
             "       const allocation_amount = [%s];\n"                                                                                                                                                                                                                                                                               \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "       let allocation_data = {};\n"                                                                                                                                                                                                                                                                                     \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "       if (allocation_array.length == allocation_amount.length) {\n"                                                                                                                                                                                                                                                    \
             "           allocation_array.forEach((element, i) => {\n"                                                                                                                                                                                                                                                                \
             "               allocation_data[element] = allocation_amount[i];\n"                                                                                                                                                                                                                                                      \
             "           });\n"                                                                                                                                                                                                                                                                                                       \
             "       }\n"                                                                                                                                                                                                                                                                                                             \
             "        // --- 1. DATA INJECTION ---\n"                                                                                                                                                                                                                                                                                 \
             "        // Ini data JSON yang lu kasih. Nanti bisa digenerate dari C.\n"                                                                                                                                                                                                                                                \
             "        const reportData = {\n"                                                                                                                                                                                                                                                                                         \
             "            period: \"%s\",\n"                                                                                                                                                                                                                                                                                          \
             "            summary: {\n"                                                                                                                                                                                                                                                                                               \
             "                income: %.2f,\n"                                                                                                                                                                                                                                                                                        \
             "                expense: %.2f,\n"                                                                                                                                                                                                                                                                                       \
             "                savings: %.2f\n"                                                                                                                                                                                                                                                                                        \
             "            },\n"                                                                                                                                                                                                                                                                                                       \
             "            dailyTrend: {\n"                                                                                                                                                                                                                                                                                            \
             "                labels: [\"1\", \"2\", \"3\", \"4\", \"5\", \"6\", \"7\", \"8\", \"9\", \"10\", \"11\", \"12\", \"13\", \"14\", \"15\", \"16\", \"17\", \"18\", \"19\", \"20\", \"21\", \"22\", \"23\", \"24\", \"25\", \"26\", \"27\", \"28\", \"29\", \"30\", \"31\"],\n"                                             \
             "                data: [%s],\n"                                                                                                                                                                                                                                                                                          \
             "                data_income: [%s]\n"                                                                                                                                                                                                                                                                                    \
             "            },\n"                                                                                                                                                                                                                                                                                                       \
             "            allocation: allocation_data,\n"                                                                                                                                                                                                                                                                             \
             "            topCategories: {\n"                                                                                                                                                                                                                                                                                         \
             "                labels: [%s],\n"                                                                                                                                                                                                                                                                                        \
             "                data: [%s]\n"                                                                                                                                                                                                                                                                                           \
             "            }\n"                                                                                                                                                                                                                                                                                                        \
             "        };\n"                                                                                                                                                                                                                                                                                                           \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "        // --- 2. HELPER FUNCTIONS ---\n"                                                                                                                                                                                                                                                                               \
             "        // Format angka jadi mata uang\n"                                                                                                                                                                                                                                                                               \
             "        const formatCurrency = (value) => {\n"                                                                                                                                                                                                                                                                          \
             "            return new Intl.NumberFormat('en-US', {\n"                                                                                                                                                                                                                                                                  \
             "                style: 'currency',\n"                                                                                                                                                                                                                                                                                   \
             "                currency: 'IDR',\n"                                                                                                                                                                                                                                                                                     \
             "                minimumFractionDigits: 2\n"                                                                                                                                                                                                                                                                             \
             "            }).format(value);\n"                                                                                                                                                                                                                                                                                        \
             "        };\n"                                                                                                                                                                                                                                                                                                           \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "        // --- 3. DOM POPULATION ---\n"                                                                                                                                                                                                                                                                                 \
             "        document.addEventListener('DOMContentLoaded', () => {\n"                                                                                                                                                                                                                                                        \
             "            // Fill Text Data\n"                                                                                                                                                                                                                                                                                        \
             "            document.getElementById('period-badge').textContent = reportData.period;\n"                                                                                                                                                                                                                                 \
             "            document.getElementById('period-text').textContent = reportData.period;\n"                                                                                                                                                                                                                                  \
             "            \n"                                                                                                                                                                                                                                                                                                         \
             "            document.getElementById('summary-income').textContent = formatCurrency(reportData.summary.income);\n"                                                                                                                                                                                                       \
             "            document.getElementById('summary-expense').textContent = formatCurrency(reportData.summary.expense);\n"                                                                                                                                                                                                     \
             "            document.getElementById('summary-savings').textContent = formatCurrency(reportData.summary.savings);\n"                                                                                                                                                                                                     \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "            document.getElementById('alloc-needs').textContent = formatCurrency(reportData.allocation.Needs);\n"                                                                                                                                                                                                        \
             "            document.getElementById('alloc-wants').textContent = formatCurrency(reportData.allocation.Wants);\n"                                                                                                                                                                                                        \
             "            document.getElementById('alloc-savings').textContent = formatCurrency(reportData.allocation.Savings);\n"                                                                                                                                                                                                    \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "            // --- 4. CHARTS CONFIG ---\n"                                                                                                                                                                                                                                                                              \
             "            \n"                                                                                                                                                                                                                                                                                                         \
             "            // A. Trend Chart (Line)\n"                                                                                                                                                                                                                                                                                 \
             "            const ctxTrend = document.getElementById('trendChart').getContext('2d');\n"                                                                                                                                                                                                                                 \
             "            new Chart(ctxTrend, {\n"                                                                                                                                                                                                                                                                                    \
             "                type: 'line',\n"                                                                                                                                                                                                                                                                                        \
             "                data: {\n"                                                                                                                                                                                                                                                                                              \
             "                    labels: reportData.dailyTrend.labels,\n"                                                                                                                                                                                                                                                            \
             "                    datasets: [{\n"                                                                                                                                                                                                                                                                                     \
             "                        label: 'Daily Expenses',\n"                                                                                                                                                                                                                                                                     \
             "                        data: reportData.dailyTrend.data,\n"                                                                                                                                                                                                                                                            \
             "                        borderColor: '#4F46E5', // Indigo\n"                                                                                                                                                                                                                                                            \
             "                        backgroundColor: 'rgba(79, 70, 229, 0.1)',\n"                                                                                                                                                                                                                                                   \
             "                        borderWidth: 2,\n"                                                                                                                                                                                                                                                                              \
             "                        fill: true,\n"                                                                                                                                                                                                                                                                                  \
             "                        tension: 0.4, // Smooth curve\n"                                                                                                                                                                                                                                                                \
             "                        pointRadius: 3,\n"                                                                                                                                                                                                                                                                              \
             "                        pointHoverRadius: 6\n"                                                                                                                                                                                                                                                                          \
             "                    },\n"                                                                                                                                                                                                                                                                                               \
             "{\n"                                                                                                                                                                                                                                                                                                                    \
             "           label: 'Daily Income',\n"                                                                                                                                                                                                                                                                                    \
             "           data: reportData.dailyTrend.data_income,\n"                                                                                                                                                                                                                                                                  \
             "           borderColor: '#4FE54E',\n"                                                                                                                                                                                                                                                                                   \
             "           backgroundColor: 'rgba(79, 229, 70, 0.1)',\n"                                                                                                                                                                                                                                                                \
             "           borderWidth: 2,\n"                                                                                                                                                                                                                                                                                           \
             "           fill: true,\n"                                                                                                                                                                                                                                                                                               \
             "           tension: 0.4, // Smooth curve\n"                                                                                                                                                                                                                                                                             \
             "           pointRadius: 3,\n"                                                                                                                                                                                                                                                                                           \
             "           pointHoverRadius: 6\n"                                                                                                                                                                                                                                                                                       \
             "       } \n"                                                                                                                                                                                                                                                                                                            \
             "]\n"                                                                                                                                                                                                                                                                                                                    \
             "                },\n"                                                                                                                                                                                                                                                                                                   \
             "                options: {\n"                                                                                                                                                                                                                                                                                           \
             "                    responsive: true,\n"                                                                                                                                                                                                                                                                                \
             "                    maintainAspectRatio: false,\n"                                                                                                                                                                                                                                                                      \
             "                    plugins: {\n"                                                                                                                                                                                                                                                                                       \
             "                        legend: { display: false },\n"                                                                                                                                                                                                                                                                  \
             "                        tooltip: {\n"                                                                                                                                                                                                                                                                                   \
             "                            mode: 'index',\n"                                                                                                                                                                                                                                                                           \
             "                            intersect: false,\n"                                                                                                                                                                                                                                                                        \
             "                        }\n"                                                                                                                                                                                                                                                                                            \
             "                    },\n"                                                                                                                                                                                                                                                                                               \
             "                    scales: {\n"                                                                                                                                                                                                                                                                                        \
             "                        y: {\n"                                                                                                                                                                                                                                                                                         \
             "                            beginAtZero: true,\n"                                                                                                                                                                                                                                                                       \
             "                            grid: { borderDash: [2, 4] }\n"                                                                                                                                                                                                                                                             \
             "                        },\n"                                                                                                                                                                                                                                                                                           \
             "                        x: {\n"                                                                                                                                                                                                                                                                                         \
             "                            grid: { display: false }\n"                                                                                                                                                                                                                                                                 \
             "                        }\n"                                                                                                                                                                                                                                                                                            \
             "                    }\n"                                                                                                                                                                                                                                                                                                \
             "                }\n"                                                                                                                                                                                                                                                                                                    \
             "            });\n"                                                                                                                                                                                                                                                                                                      \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "            // B. Allocation Chart (Doughnut)\n"                                                                                                                                                                                                                                                                        \
             "            const ctxAlloc = document.getElementById('allocationChart').getContext('2d');\n"                                                                                                                                                                                                                            \
             "            new Chart(ctxAlloc, {\n"                                                                                                                                                                                                                                                                                    \
             "                type: 'doughnut',\n"                                                                                                                                                                                                                                                                                    \
             "                data: {\n"                                                                                                                                                                                                                                                                                              \
             "                    labels: ['Needs', 'Wants', 'Savings'],\n"                                                                                                                                                                                                                                                           \
             "                    datasets: [{\n"                                                                                                                                                                                                                                                                                     \
             "                        data: [\n"                                                                                                                                                                                                                                                                                      \
             "                            reportData.allocation.Needs,\n"                                                                                                                                                                                                                                                             \
             "                            reportData.allocation.Wants,\n"                                                                                                                                                                                                                                                             \
             "                            reportData.allocation.Savings\n"                                                                                                                                                                                                                                                            \
             "                        ],\n"                                                                                                                                                                                                                                                                                           \
             "                        backgroundColor: [\n"                                                                                                                                                                                                                                                                           \
             "                            '#3B82F6', // Blue\n"                                                                                                                                                                                                                                                                       \
             "                            '#F59E0B', // Amber\n"                                                                                                                                                                                                                                                                      \
             "                            '#10B981'  // Emerald\n"                                                                                                                                                                                                                                                                    \
             "                        ],\n"                                                                                                                                                                                                                                                                                           \
             "                        borderWidth: 0,\n"                                                                                                                                                                                                                                                                              \
             "                        hoverOffset: 4\n"                                                                                                                                                                                                                                                                               \
             "                    }]\n"                                                                                                                                                                                                                                                                                               \
             "                },\n"                                                                                                                                                                                                                                                                                                   \
             "                options: {\n"                                                                                                                                                                                                                                                                                           \
             "                    responsive: true,\n"                                                                                                                                                                                                                                                                                \
             "                    maintainAspectRatio: false,\n"                                                                                                                                                                                                                                                                      \
             "                    cutout: '70%',\n"                                                                                                                                                                                                                                                                                   \
             "                    plugins: {\n"                                                                                                                                                                                                                                                                                       \
             "                        legend: { position: 'bottom' }\n"                                                                                                                                                                                                                                                               \
             "                    }\n"                                                                                                                                                                                                                                                                                                \
             "                }\n"                                                                                                                                                                                                                                                                                                    \
             "            });\n"                                                                                                                                                                                                                                                                                                      \
             "\n"                                                                                                                                                                                                                                                                                                                     \
             "            // C. Top Categories (Horizontal Bar)\n"                                                                                                                                                                                                                                                                    \
             "            const ctxCat = document.getElementById('categoryChart').getContext('2d');\n"                                                                                                                                                                                                                                \
             "            new Chart(ctxCat, {\n"                                                                                                                                                                                                                                                                                      \
             "                type: 'bar',\n"                                                                                                                                                                                                                                                                                         \
             "                data: {\n"                                                                                                                                                                                                                                                                                              \
             "                    labels: reportData.topCategories.labels,\n"                                                                                                                                                                                                                                                         \
             "                    datasets: [{\n"                                                                                                                                                                                                                                                                                     \
             "                        label: 'Amount',\n"                                                                                                                                                                                                                                                                             \
             "                        data: reportData.topCategories.data,\n"                                                                                                                                                                                                                                                         \
             "                        backgroundColor: '#6366F1', // Indigo 500\n"                                                                                                                                                                                                                                                    \
             "                        borderRadius: 4,\n"                                                                                                                                                                                                                                                                             \
             "                        barThickness: 20\n"                                                                                                                                                                                                                                                                             \
             "                    }]\n"                                                                                                                                                                                                                                                                                               \
             "                },\n"                                                                                                                                                                                                                                                                                                   \
             "                options: {\n"                                                                                                                                                                                                                                                                                           \
             "                    indexAxis: 'y', // Horizontal Bar\n"                                                                                                                                                                                                                                                                \
             "                    responsive: true,\n"                                                                                                                                                                                                                                                                                \
             "                    maintainAspectRatio: false,\n"                                                                                                                                                                                                                                                                      \
             "                    plugins: {\n"                                                                                                                                                                                                                                                                                       \
             "                        legend: { display: false }\n"                                                                                                                                                                                                                                                                   \
             "                    },\n"                                                                                                                                                                                                                                                                                               \
             "                    scales: {\n"                                                                                                                                                                                                                                                                                        \
             "                        x: {\n"                                                                                                                                                                                                                                                                                         \
             "                            grid: { borderDash: [2, 4] }\n"                                                                                                                                                                                                                                                             \
             "                        },\n"                                                                                                                                                                                                                                                                                           \
             "                        y: {\n"                                                                                                                                                                                                                                                                                         \
             "                            grid: { display: false }\n"                                                                                                                                                                                                                                                                 \
             "                        }\n"                                                                                                                                                                                                                                                                                            \
             "                    }\n"                                                                                                                                                                                                                                                                                                \
             "                }\n"                                                                                                                                                                                                                                                                                                    \
             "            });\n"                                                                                                                                                                                                                                                                                                      \
             "        });\n"                                                                                                                                                                                                                                                                                                          \
             "    </script>\n"                                                                                                                                                                                                                                                                                                        \
             "</body>\n"

// untuk pembatas akhir cuma ini doang dulu
#define FOOTER "</html>"

// helper buat menginversi int ke nama bulan
static char *get_month_name(int month)
{
    switch (month)
    {
    case 1:
        return "January";
        break;
    case 2:
        return "February";
        break;
    case 3:
        return "March";
        break;
    case 4:
        return "April";
        break;
    case 5:
        return "May";
        break;
    case 6:
        return "June";
        break;
    case 7:
        return "July";
        break;
    case 8:
        return "August";
        break;
    case 9:
        return "September";
        break;
    case 10:
        return "October";
        break;
    case 11:
        return "November";
        break;
    case 12:
        return "December";
        break;
    default:
        return "";
        break;
    }
}

// helper untuk mengonversi data double ke string
static char *concat_double_array_to_string(double *arr, int size, const char *separator)
{
    if (arr == NULL || size <= 0)
    {
        return strdup(""); // Return an empty string if array is invalid
    }

    // Estimate maximum length for each double + separator + null terminator
    // A reasonable estimate for a double might be around 20 characters (including sign, decimal, and exponent)
    // Plus the length of the separator.
    // tapi karena ku kurangi jadi .2 data di belakang decimal point jadi ku kurangi juga 5 byte awalnya 26 btw
    int estimated_len_per_double = 21;
    size_t total_len = size * estimated_len_per_double + (size > 0 ? (size - 1) * strlen(separator) : 0) + 1; // +1 for null terminator

    char *result_string = (char *)malloc(total_len);
    if (result_string == NULL)
    {
        perror("Failed to allocate memory for result_string");
        return NULL;
    }
    result_string[0] = '\0'; // Initialize as an empty string

    char temp_buffer[estimated_len_per_double]; // Temporary buffer for converting each double

    for (int i = 0; i < size; i++)
    {
        snprintf(temp_buffer, sizeof(temp_buffer), "%.2f", arr[i]); // Convert double to string with 6 decimal places
        strcat(result_string, temp_buffer);

        if (i < size - 1)
        { // Add separator if not the last element
            strcat(result_string, separator);
        }
    }

    return result_string;
}

// helper untuk mengonversi array string ke string yang ada '' untuk bnisa dibaca js
static char *concat_array_strings_with_quotes(char *strings[], int count, const char *separator)
{
    if (count == 0)
    {
        char *empty = malloc(1);
        if (empty)
            *empty = '\0';
        return empty;
    }
    // 1. Calculate total length
    size_t total_length = 0;
    size_t sep_len = strlen(separator);

    for (int i = 0; i < count; i++)
    {
        total_length += strlen(strings[i]);
    }

    // Add space for quotes (2 per string) + separators + null terminator
    total_length += (count * 2);           // ' and ' for each element
    total_length += (count - 1) * sep_len; // Separators between elements
    total_length += 1;                     // Null terminator

    // 2. Allocate memory
    char *result = malloc(total_length);
    if (!result)
        return NULL;

    // 3. Construct the string using a moving pointer
    char *ptr = result;

    for (int i = 0; i < count; i++)
    {
        // Add separator (only if it's not the first element)
        if (i > 0)
        {
            strcpy(ptr, separator);
            ptr += sep_len;
        }

        // Write the string wrapped in quotes to the current pointer location
        // sprintf returns the number of bytes written, so we add that to ptr
        ptr += sprintf(ptr, "'%s'", strings[i]);
    }

    return result;
}

// helper buat menghitung sum
static double get_sum(sqlite3 *db, const char *sql, int user_id, const char *start_date, const char *end_date)
{
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, start_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, end_date, -1, SQLITE_STATIC);

    double result = 0.0;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        result = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return result;
}

// return err status
int generate_report(sqlite3 *db, User *user, int month, int year)
{
    // coba baca file nya
    FILE *f = fopen("report.html", "w");

    if (f == NULL)
    {
        fprintf(stderr, "Failed to open report.html for writing.\n");
        return 1;
    }

    // Cal date range
    char start_date[11], end_date[11];
    snprintf(start_date, sizeof(start_date), "%04d-%02d-01", year, month);
    snprintf(end_date, sizeof(start_date), "%04d-%02d-31", year, month);

    // cal expenses and income
    double total_income = get_sum(db, "SELECT SUM(amount) as total FROM transactions WHERE user_id = ? AND date(transaction_date) BETWEEN date(?) AND date(?) AND type = 'income'", user->id, start_date, end_date);
    double total_expense = get_sum(db, "SELECT SUM(amount) as total FROM transactions WHERE user_id = ? AND date(transaction_date) BETWEEN date(?) AND date(?) AND type = 'expense'", user->id, start_date, end_date);
    double total_balance = total_income - total_expense;

    // change to verbose for print
    char month_year_verbose[24];
    snprintf(month_year_verbose, sizeof(month_year_verbose), "%s %d", get_month_name(month), year);

    sqlite3_stmt *stmt;

    const char *income_sql = "SELECT COALESCE(SUM(amount), 0) FROM transactions WHERE user_id = ? AND date(transaction_date) = ? AND type = 'income'";

    if (sqlite3_prepare_v2(db, income_sql, -1, &stmt, 0) != SQLITE_OK)
    {
        printf("Prepare Error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    double income_per_day[31];
    double expense_per_day[31];

    // Initialize arrays to 0 to be safe
    for (int k = 0; k < 31; k++)
    {
        income_per_day[k] = 0;
        expense_per_day[k] = 0;
    }

    // --- PROCESS INCOME ---
    for (int i = 1; i <= 31; i++)
    {
        double amount = 0;
        char date_str[11];
        // Ensure format matches your DB (YYYY-MM-DD)
        snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d", year, month, i);

        sqlite3_bind_int(stmt, 1, user->id);
        sqlite3_bind_text(stmt, 2, date_str, -1, SQLITE_TRANSIENT);

        int err = sqlite3_step(stmt);

        if (err == SQLITE_ROW)
        {
            // FIX 2: Even if SUM returns NULL (no rows), column_double returns 0.0
            amount = sqlite3_column_double(stmt, 0);
        }
        else if (err != SQLITE_DONE)
        {
            printf("Step Error (Income day %d): %s\n", i, sqlite3_errmsg(db));
        }
        // FIX 3: Removed the 'break'. If err == SQLITE_DONE (no data),
        // we just leave amount at 0 and continue to the next day.

        income_per_day[i - 1] = amount;

        sqlite3_reset(stmt); // Reset stmt for next iteration
    }

    // FIX 4: Finalize the first statement before reusing the variable!
    sqlite3_finalize(stmt);

    // --- PROCESS EXPENSE ---
    const char *expense_sql = "SELECT COALESCE(SUM(amount), 0) FROM transactions WHERE user_id = ? AND date(transaction_date) = ? AND type = 'expense'";

    if (sqlite3_prepare_v2(db, expense_sql, -1, &stmt, 0) != SQLITE_OK)
    {
        printf("Prepare Error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    for (int i = 1; i <= 31; i++)
    {
        double amount = 0;
        char date_str[11];
        snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d", year, month, i);

        sqlite3_bind_int(stmt, 1, user->id);
        sqlite3_bind_text(stmt, 2, date_str, -1, SQLITE_TRANSIENT);

        int err = sqlite3_step(stmt);

        if (err == SQLITE_ROW)
        {
            amount = sqlite3_column_double(stmt, 0);
        }
        else if (err != SQLITE_DONE)
        {
            printf("Step Error (Expense day %d): %s\n", i, sqlite3_errmsg(db));
        }

        expense_per_day[i - 1] = amount;

        sqlite3_reset(stmt);
    }

    // Finalize the second statement
    sqlite3_finalize(stmt);

    // ubah data yang tadi kedalam bentuk string
    char *concat_income_per_day = concat_double_array_to_string(income_per_day, 31, ",");
    char *concat_expense_per_day = concat_double_array_to_string(expense_per_day, 31, ",");

    // ambil data persen alokasi
    const char *allocation_percentage_sql = "SELECT Categories.budget_bucket, COALESCE(SUM(amount), 0) FROM transactions JOIN Categories ON transactions.category_id = Categories.id WHERE user_id = ? AND date(transaction_date) BETWEEN date(?) AND date(?) AND Categories.type = 'expense' GROUP BY Categories.budget_bucket ORDER BY SUM(amount) DESC";

    if (sqlite3_prepare_v2(db, allocation_percentage_sql, -1, &stmt, 0) != SQLITE_OK)
    {
        printf("Prepare Error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_int(stmt, 1, user->id);
    sqlite3_bind_text(stmt, 2, start_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, end_date, -1, SQLITE_STATIC);

    int rc;

    int count = 0;
    char **allocation_name = NULL;
    double *allocation_amount = NULL;

    do
    {
        rc = sqlite3_step(stmt);

        switch (rc)
        {
        case SQLITE_DONE:
            break;
        case SQLITE_ROW:

            char **temp = realloc(allocation_name, sizeof(char *) * (count + 1));
            double *temp2 = realloc(allocation_amount, sizeof(double) * (count + 1));

            if (temp == NULL)
            {
                fprintf(stderr, "Memory allocation failed for array\n");
                free(allocation_name);
                exit(1);
            }

            allocation_name = temp;
            allocation_amount = temp2;

            allocation_name[count] = (char *)malloc(sizeof(char) * 26);

            if (allocation_name[count] == NULL)
            {
                fprintf(stderr, "Memory allocation failed: %s\n", sqlite3_errmsg(db));
                free(allocation_name);
                sqlite3_finalize(stmt);
                exit(1);
            }

            const unsigned char *text = sqlite3_column_text(stmt, 0);
            if (text)
            {
                strncpy(allocation_name[count], (const char *)text, sizeof(allocation_name[count]) - 1);
                allocation_name[count][sizeof(allocation_name[count]) - 1] = '\0';
            }
            else
            {
                allocation_name[count][0] = '\0';
            }

            allocation_amount[count] = sqlite3_column_double(stmt, 1);

            count++;
            break;
        default:
            fprintf(stderr, "Error on retriving row: %s\n", sqlite3_errmsg(db));
            break;
        }

    } while (rc == SQLITE_ROW);

    // ubah lagi kedalam bentuk string
    char *concat_allocation_name = concat_array_strings_with_quotes(allocation_name, count, ",");
    char *concat_allocation_amount = concat_double_array_to_string(allocation_amount, count, ",");

    sqlite3_finalize(stmt);

    // lakukan hal yang sama dengan query yang berbeda untuk mendapatkan data per category
    const char *categories_spending_sql = "SELECT Categories.name, COALESCE(SUM(amount), 0) FROM transactions JOIN Categories ON transactions.category_id = Categories.id WHERE user_id = ? AND date(transaction_date) BETWEEN date(?) AND date(?) AND Categories.type = 'expense' GROUP BY Categories.name ORDER BY SUM(amount) DESC";

    if (sqlite3_prepare_v2(db, categories_spending_sql, -1, &stmt, 0) != SQLITE_OK)
    {
        printf("Prepare Error: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_bind_int(stmt, 1, user->id);
    sqlite3_bind_text(stmt, 2, start_date, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, end_date, -1, SQLITE_STATIC);

    int c_count = 0;
    char **categories_spending_name = NULL;
    double *categories_spending_amount = NULL;

    do
    {
        rc = sqlite3_step(stmt);

        switch (rc)
        {
        case SQLITE_DONE:
            break;
        case SQLITE_ROW:

            char **temp = realloc(categories_spending_name, sizeof(char *) * (c_count + 1));
            double *temp2 = realloc(categories_spending_amount, sizeof(double) * (c_count + 1));

            if (temp == NULL)
            {
                fprintf(stderr, "Memory allocation failed for array\n");
                free(categories_spending_name);
                free(categories_spending_amount);
                exit(1);
            }

            categories_spending_name = temp;
            categories_spending_amount = temp2;

            categories_spending_name[c_count] = (char *)malloc(sizeof(char) * 26);

            if (categories_spending_name[c_count] == NULL)
            {
                fprintf(stderr, "Memory allocation failed: %s\n", sqlite3_errmsg(db));
                free(categories_spending_name);
                sqlite3_finalize(stmt);
                exit(1);
            }

            const unsigned char *text = sqlite3_column_text(stmt, 0);
            if (text)
            {
                categories_spending_name[c_count] = strdup(text);
            }
            else
            {
                categories_spending_name[c_count] = strdup("\0");
            }

            categories_spending_amount[c_count] = sqlite3_column_double(stmt, 1);

            c_count++;
            break;
        default:
            fprintf(stderr, "Error on retriving row: %s\n", sqlite3_errmsg(db));
            break;
        }

    } while (rc == SQLITE_ROW);

    char *concat_expense_per_category_names = concat_array_strings_with_quotes(categories_spending_name, c_count, ",");
    char *concat_expense_per_category_amount = concat_double_array_to_string(categories_spending_amount, c_count, ",");

    // sisipkan data sesuai urutan
    fprintf(f, HEADER, user->username);
    fprintf(f, BODY, concat_allocation_name, concat_allocation_amount, month_year_verbose, total_income, total_expense, total_balance, concat_expense_per_day, concat_income_per_day, concat_expense_per_category_names, concat_expense_per_category_amount);
    fprintf(f, FOOTER);

    // tutup file
    fclose(f);

    printf("Report generated successfully. at ./report.html\n");
    printf("Open ./report.html in your browser to view the report.\n");
    printf("Press Enter to continue...'\n");
    getchar();

    // beberes memory yang di pakai
    free(concat_income_per_day);
    free(concat_expense_per_day);

    for (size_t i = 0; i < count; i++)
    {
        free(allocation_name[i]);
    }

    for (size_t i = 0; i < c_count; i++)
    {
        free(categories_spending_name[i]);
    }

    free(allocation_name);
    free(allocation_amount);
    free(categories_spending_name);
    free(categories_spending_amount);
    free(concat_expense_per_category_names);
    free(concat_expense_per_category_amount);
    free(concat_allocation_name);
    free(concat_allocation_amount);

    return 0;
}