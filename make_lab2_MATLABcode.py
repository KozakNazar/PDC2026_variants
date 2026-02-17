####################################
# N.Kozak // Lviv'2026             #
#    file: make_lab2_MATLABcode.py #
####################################
import re
from pathlib import Path

from make_labs_common_functions import replace_skip, convert_MATLAB_formula, parse_variants_file, lab2_variants_redef

def make_lab2_matlab_script(variants_data, year, group, variant_number):
    variant = None
    for v in variants_data:
        if v['number'] == variant_number:
            variant = v
            break
    
    if not variant:
        return None
    
    main_formula = variant['formula']
    b_i_formula = variant['b_i']
    y2_formula = variant['y2']
    Y3_formula = variant['Y3']
    C2_ij_formula = variant['C2_ij']
    
    b_formula_even = "1/(i^2 + 2)"  # за замовчуванням
    b_formula_odd = "1/i"  # за замовчуванням
    
    if "для парних" in b_i_formula:
        parts = b_i_formula.split("для парних")
        b_formula_even = parts[0].replace("b_{i}=", "").strip()
        
        if "для непарних" in b_i_formula:
            parts2 = b_i_formula.split("для непарних")
            b_formula_odd = parts2[0].split(";")[1].replace("b_{i}=", "").strip()
    else:
        b_formula_even = b_i_formula.replace("b_{i}=", "").strip()
        b_formula_odd = b_formula_even

    if "C_{2ij}=" in C2_ij_formula:
        C2_ij_formula = C2_ij_formula.split("C_{2ij}=")[1].strip()
    else:
        C2_ij_formula = "1/(i + j)"
    
    matlab_code = f"""%% Варіант №{variant_number}
% Обчислення виразу: {convert_MATLAB_formula(variant['formula'])}

clear all; close all; clc;

DEFAULT_N = 2;
CHOICE_MANUAL = 1;
CHOICE_RAND = 2;

RESULT_TYPE = '{variant['type']}';

try
%% 1. Ввід розмірності
    n = input('Введіть розмірність n: ');
    while n <= 0
        n = input('Розмірність повинна бути > 0. Введіть n: ');
    end

%% 2. Вибір способу вводу даних
    choice = 0;
    disp('Виберіть спосіб вводу даних:');
    while n != CHOICE_MANUAL && n != CHOICE_RAND
        n = input('%d - ввід з клавіатури, %d - випадкова генерація: ', CHOICE_MANUAL, CHOICE_RAND);
    end
catch
    n = DEFAULT_N;
    choice = CHOICE_RAND;
    printf('%d (значення за замовчуванням, ввід не підтримується системою виконання MATLAB-коду)\\n', n);
end

%% 3. Створення матриць та векторів
if choice == 1
    %% Ввід з клавіатури
    disp('=== Ввід матриці A ===');
    A = zeros(n);
    for i = 1:n
        for j = 1:n
            A(i,j) = input(sprintf('A(%d,%d) = ', i, j));
        end
    end
    
    disp('=== Ввід матриці A1 ===');
    A1 = zeros(n);
    for i = 1:n
        for j = 1:n
            A1(i,j) = input(sprintf('A1(%d,%d) = ', i, j));
        end
    end
    
    disp('=== Ввід матриці A2 ===');
    A2 = zeros(n);
    for i = 1:n
        for j = 1:n
            A2(i,j) = input(sprintf('A2(%d,%d) = ', i, j));
        end
    end
    
    disp('=== Ввід матриці B2 ===');
    B2 = zeros(n);
    for i = 1:n
        for j = 1:n
            B2(i,j) = input(sprintf('B2(%d,%d) = ', i, j));
        end
    end
    
    disp('=== Ввід вектора b1 ===');
    b1 = zeros(n);
    for i = 1:n
        b1(i) = input(sprintf('b1(%d) = ', i));
    end

    disp('=== Ввід вектора c1 ===');
    c1 = zeros(n);
    for i = 1:n
        c1(i) = input(sprintf('c1(%d) = ', i));
    end
else
    %% Випадкова генерація
    disp('Генеруються випадкові матриці та вектори...');
    A = randi([1, 9], n, n);
    A1 = randi([1, 9], n, n)
    A2 = randi([1, 9], n, n)
    B2 = randi([1, 9], n, n)
    b1 = randi([1, 9], n, 1);
    c1 = randi([1, 9], n, 1);
    
    disp('Згенеровані матриці та вектори:');
    disp('A = '); disp(A);
    disp('A1 = '); disp(A1);
    disp('A2 = '); disp(A2);
    disp('B2 = '); disp(B2);
    disp('b1 = '); disp(b1);
    disp('c1 = '); disp(c1);
end

%% 4. Обчислення вектора b згідно формули
disp('Обчислення вектора b...');
b = zeros(n, 1);
for i = 1:n
    % Формула для b_i
    if mod(i, 2) == 0
        b(i) = {convert_MATLAB_formula(b_formula_even)}; % для парних i
    else
        b(i) = {convert_MATLAB_formula(b_formula_odd)}; % для непарних i
    end
    %% b(i) = b_i_val;
end

disp('Вектор b:');
disp(b);

%% 5. Обчислення y1 = A*b
y1 = A * b;
disp('Вектор y1 = A*b:');
disp(y1);

%% 6. Обчислення y2:
disp('Обчислення y2...');
% Формула містить A1, b1, c1
y2 = {convert_MATLAB_formula(y2_formula)};

disp('Вектор y2:');
disp(y2);

%% 7. Обчислення матриці C2:
disp('Обчислення матриці C2...');
C2 = zeros(n);
for i = 1:n
    for j = 1:n
        C2(i,j) = {convert_MATLAB_formula(C2_ij_formula)};
    end
end

disp('Матриця C2:');
disp(C2);

%% 8. Обчислення Y3:
disp('Обчислення матриці Y3...');
Y3 = {convert_MATLAB_formula(Y3_formula)};

disp('Матриця Y3:');
disp(Y3);

%% 9. Обчислення x:
disp('Обчислення x...');
{convert_MATLAB_formula(main_formula)};

[r, c] = size(x);

printf('Результат ');
if strcmp(RESULT_TYPE, 'матриця') && r == n && c == n
    printf('матриця'); 
elseif strcmp(RESULT_TYPE, 'стовпець') && r == n && c == 1
    printf('стовпець');
elseif strcmp(RESULT_TYPE, 'рядок') && r == 1 && c == n
    printf('рядок');
elseif strcmp(RESULT_TYPE, 'число') && r == 1 && c == 1
    printf('число');
else 
    printf('неочікуваний формат');
end

printf(' x:\\n');
disp(x);
printf('({year - 1}/{year} н.р., KI-{str(group)}, варіант №{variant_number}: {convert_MATLAB_formula(variant['formula']).replace("'", "''")})');
"""
    
    return matlab_code

def main():
    YEAR_FIRST = 2026
    YEAR_LAST = 2027
    GROUP_FIRST = 301
    GROUP_LAST = 309
    LAB2_VARIANTS_DATA_FILE_NAME = "lab2_variants_data.txt"

    # Читаємо базові варіанти
    variants_ = parse_variants_file(LAB2_VARIANTS_DATA_FILE_NAME)

    for year in range(YEAR_FIRST, YEAR_LAST + 1):
        for group in range(GROUP_FIRST, GROUP_LAST + 1):    
            # Створюємо нові варіанти
            variants = lab2_variants_redef(variants_, year, group)
    
            if not variants:
                print("Не знайдено варіантів у файлі")
                return
    
            for variant_num in range(1, 30 + 1):
                # Генеруємо MATLAB код
                matlab_code = make_lab2_matlab_script(variants, year, group, variant_num)
    
                if matlab_code:                    
                    #  Створюємо шлях до файлу
                    filename = f"PRO_LAB2_VARIANTSANDMATLAB/{year - 1}_{year}/KI{group}/{year - 1}{year}_KI{group}_MATLABscripts/l2_{year - 1}{year}_ki{group}_{variant_num}_MATLAB.m"
                    path = Path(filename)
                    path.parent.mkdir(parents=True, exist_ok=True) # if path.parent != Path("."):

                    # Зберігаємо у файл
                    with open(filename, 'w', encoding='utf-8') as f:
                        f.write(matlab_code)
                        print("Created:", filename)

    print(f"\nMATLAB скрипти згенеровано: тека PRO_LAB2_VARIANTSANDMATLAB")
    print(f"\nІнструкція:")
    print(f"1. Відкрийте один із скриптів у MATLAB")
    print(f"2. Запустіть його")
    print(f"3. Дотримуйтесь інструкцій на екрані")

if __name__ == "__main__":
    main()