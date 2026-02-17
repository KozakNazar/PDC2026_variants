#####################################################
# N.Kozak // Lviv'2026                              #
#    file: make_lab2_MATLABcode_for_simple_variants #
#####################################################
import re
from pathlib import Path

from make_labs_common_functions import replace_skip, convert_MATLAB_formula, parse_variants_file, lab2_variants_redef

def make_lab2_matlab_script_for_simple_variants(variants_data, year, group, variant_number):
    variant = None
    for v in variants_data:
        if v['number'] == variant_number:
            variant = v
            break
    
    if not variant:
        return None
    
    main_formula = variant['formula']
    
    matlab_code = f"""%% Варіант №{variant_number}
% Обчислення виразу: {convert_MATLAB_formula(variant['formula'])}

clear all; close all; clc;

DEFAULT_N = 7;
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

%% 3. Створення векторів та матриць 
if choice == 1
    %% Ввід з клавіатури  
    disp('=== Ввід вектора y1 ===');
    y1 = zeros(n);
    for index = index:n
        b1(index) = input(sprintf('y1(%d) = ', index));
    end

    disp('=== Ввід вектора y2 ===');
    y2 = zeros(n);
    for index = 1:n
        c1(index) = input(sprintf('y2(%d) = ', index));
    end

    disp('=== Ввід матриці Y3 ===');
    Y3 = zeros(n);
    for iIndex = 1:n
        for jIndex = 1:n
            Y3(iIndex,jIndex) = input(sprintf('Y3(%d,%d) = ', iIndex, jIndex));
        end
    end
else
    %% Випадкова генерація
    disp('Генеруються випадкові вектори та матриці...');
    y1 = randi([1, 9], n, 1);
    y2 = randi([1, 9], n, 1);
    Y3 = randi([1, 9], n, n);
    
    disp('Згенеровані вектори та матриці:');
    disp('y1 = '); disp(y1);
    disp('y2 = '); disp(y2);
    disp('Y3 = '); disp(Y3);
end

%% 3. Обчислення x:
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
    LAB2_VARIANTS_DATA_FILE_NAME = "lab2_simple_variants_data.txt"

    # Читаємо базові варіанти
    variants_ = parse_variants_file(LAB2_VARIANTS_DATA_FILE_NAME, True)

    for year in range(YEAR_FIRST, YEAR_LAST + 1):
        for group in range(GROUP_FIRST, GROUP_LAST + 1):    
            # Створюємо нові варіанти
            variants = lab2_variants_redef(variants_, year, group, True)
    
            if not variants:
                print("Не знайдено варіантів у файлі")
                return
    
            for variant_num in range(1, 30 + 1):
                # Генеруємо MATLAB код
                matlab_code = make_lab2_matlab_script_for_simple_variants(variants, year, group, variant_num)
    
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