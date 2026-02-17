##########################################
# N.Kozak // Lviv'2026                   #
#    file: make_labs_common_functions.py #
##########################################
import re
import shutil
import os

def replace_skip(text, old, new, skip=0, count=None):
    occ = 0 
    def repl(match):
        nonlocal occ
        occ += 1
        if occ <= skip:
            return match.group(0)
        if count is not None and occ > skip + count:
            return match.group(0)
        return new
    
    return re.sub(re.escape(old), repl, text)

def convert_to_math_unicode_formula(text, useTSymbol=False, useStarSymbol=True):
    if not useStarSymbol:
        text = text.replace(' * ', '')
        text = text.replace(' *', '')
        text = text.replace('* ', '')
        text = text.replace('*', '')
    text = text.replace('_{1}', '₁')#.replace('_1', '₁')
    text = text.replace('_{2}', '₂')#.replace('_2', '₂')
    text = text.replace('_{3}', '₃')#.replace('_3', '₃')
    text = text.replace('^{2}', '²')#.replace('^2', '²')
    text = text.replace('^{3}', '³')#.replace('^3', '³')
    text = text.replace('^{4}', '⁴')#.replace('^4', '⁴')
    text = text.replace('_{i}', 'ᵢ')#.replace('_i', 'ᵢ')
    text = text.replace('_{j}', 'ⱼ')#.replace('_j', 'ⱼ')
    text = text.replace('_{ij}', 'ᵢⱼ')#.replace('_ij', 'ᵢⱼ')
    text = text.replace('_{2ij}', '₂ᵢⱼ')#.replace('_2ij', '₂ᵢⱼ')
    if useTSymbol:
        text = text.replace("^{'}", "ᵀ")
    else:
        text = text.replace("^{'}", "'")
    
    return text

def convert_MATLAB_formula(formula):
    formula = formula.replace('_{1}', '1')#.replace('_1', '1')
    formula = formula.replace('_{2}', '2')#.replace('_2', '2')
    formula = formula.replace('_{3}', '3')#.replace('_3', '3')
    formula = formula.replace('^{2}', '^2')
    formula = formula.replace('^{3}', '^3')
    formula = formula.replace('^{4}', '^4')
    formula = formula.replace('_{i}', '(i)')#.replace('_i', '(i)')
    formula = formula.replace('_{j}', '(j)')#.replace('_j', '(j)')
    formula = formula.replace('_{ij}', '(i,j)')#.replace('_ij', '(i,j)')
    formula = formula.replace('_{2ij}', '2(i,j)')#.replace('_2ij', '2(i,j)')
    formula = formula.replace("^{'}", "'")
    
    return formula

def parse_variants_file(filename, simple_variants=False):
    variants = []
    with open(filename, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            
            parts = line.split('|')
            if len(parts) >= 3:
                number = int(parts[0])
                formula = parts[1]
                type_text = parts[2]
                if not simple_variants and len(parts) >= 7:
                    b_i = parts[3]
                    y2 = parts[4]
                    Y3 = parts[5]
                    C2_ij = parts[6]

            variants.append({
                'number': number,
                'formula': formula,
                'type': type_text,
                **({} if simple_variants or len(parts) < 7 else {  # тут else логічний - це частина виразу
                    'b_i': b_i,
                    'y2': y2,
                    'Y3': Y3,
                    'C2_ij': C2_ij
                })
            })
                
    return variants

def lab2_variants_redef(variants_data, year, group, simple_variants=False):
    new_variants_data = variants_data

    param_Y_formula_and_type = 17 * (year - 2025);
    param_Y_b_i = 13 * (year - 2025);
    param_Y_y2 = 11 * (year - 2025);
    param_Y_Y3 = 7 * (year - 2025);
    param_Y_C2_ij = 5 * (year - 2025);

    variant_count = len(variants_data);
    for index in range(variant_count):
        new_variants_data[index]['number'] = variants_data[index]['number'];
        new_variants_data[index]['formula'] = variants_data[(index + param_Y_formula_and_type)%variant_count]['formula'];
        new_variants_data[index]['type'] = variants_data[(index + param_Y_formula_and_type)%variant_count]['type'];
        if not simple_variants:
            new_variants_data[index]['b_i'] = variants_data[(index + param_Y_b_i)%variant_count]['b_i'];
            new_variants_data[index]['y2'] = variants_data[(index + param_Y_y2)%variant_count]['y2'];
            new_variants_data[index]['Y3'] = variants_data[(index + param_Y_Y3)%variant_count]['Y3'];
            new_variants_data[index]['C2_ij'] = variants_data[(index + param_Y_C2_ij)%variant_count]['C2_ij'];
        
    param_G_A = (group - 300) & 1;
    param_G_B = ((group - 300) & 2) >> 1; # !
    param_G_C = ((group - 300) & 4) >> 2; # !
    param_G_D = ((group - 300) & 8) >> 3; # !

    for v in new_variants_data:
        v['formula'] = replace_skip(v['formula'], '^{2}', '^{2_}', 1 - param_G_A, 2 - param_G_A);
        v['formula'] = replace_skip(v['formula'], '^{3}', '^{2}', 1 - param_G_A, 2 - param_G_A);
        v['formula'] = replace_skip(v['formula'], '^{2_}', '^{3}');

        if simple_variants:
            v['formula'] = replace_skip(v['formula'], '_{1}', '_{1_}', 1 - param_G_B, 2 - param_G_B);
            v['formula'] = replace_skip(v['formula'], '_{2}', '_{1}', 1 - param_G_B, 2 - param_G_B);
            v['formula'] = replace_skip(v['formula'], '_{1_}', '_{2}');
        
        v['formula'] = replace_skip(v['formula'], '+', '+_', 1 - param_G_C, 2 - param_G_D);
        v['formula'] = replace_skip(v['formula'], '-', '+', 1 - param_G_C, 2 - param_G_D);
        v['formula'] = replace_skip(v['formula'], '+_', '-');

    return new_variants_data

import math

def get_permutation_count(n, k):
    """
    Обчислює кількість перестановок
    
    Args:
        n: кількість елементів
        k: доступні елементи
    
    Returns:
        Кількість перестановк
    """
    
    return math.factorial(n) // math.factorial(n - k)


def get_permutation_by_number(num, n, digits=None):
    """
    Обчислює перестановку за її номером
    
    Args:
        num: номер перестановки (0-based)
        n: довжина перестановки
        digits: доступні цифри
    
    Returns:
        Перестановка з номером num
    """
    if digits is None:
        digits = list(range(10))
    
    digits = sorted(digits)
    if n > len(digits):
        raise ValueError(f"n ({n}) > кількість цифр ({len(digits)})")
    
    result = []
    available = list(digits)
    
    for i in range(n):
        fact = math.factorial(n - i - 1)
        index = num // fact
        num %= fact
        
        result.append(available.pop(index))
    
    return tuple(result)

def permutation_number(permutation, digits=None):
    """
    Обчислює номер перестановки в лексикографічному порядку
    
    Args:
        perm: перестановка (кортеж або список)
        digits: всі доступні цифри у порядку сортування
    
    Returns:
        Номер перестановки (починаючи з 0)
    """
    if digits is None:
        digits = sorted(permutation)  # цифри у порядку зростання
    
    n = len(permutation)
    total = 0
    
    # Копіюємо список доступних цифр
    available = list(digits)
    
    for i, digit in enumerate(permutation):
        # Знаходимо позицію цієї цифри серед доступних
        pos = available.index(digit)
        
        # Кількість перестановок, що починаються з менших цифр
        total += pos * math.factorial(n - i - 1)
        
        # Видаляємо використану цифру
        available.remove(digit)
    
    return total

def n_copy(n, src):
    for index in range(1, n + 1):
        dst = f"{index}_{src}"
        shutil.copyfile(src, dst)

    print(f"Cтворено копії 2_{src} … 30_{src} файлу {src}.")