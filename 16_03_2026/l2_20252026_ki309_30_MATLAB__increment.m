%% Варіант №30
% Обчислення виразу: x = y2' * y1 * Y3^3 + Y3 * (y2 * y1') * Y3 - Y3^2

clear all; close all; clc;

DEFAULT_N = 7;
CHOICE_MANUAL = 1;
CHOICE_RAND = 2;

RESULT_TYPE = 'матриця';

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
        n = input('%d - ввід з клавіатури, %d - генерація: ', CHOICE_MANUAL, CHOICE_RAND);
    end
catch
    n = DEFAULT_N;
    choice = CHOICE_RAND;
    printf('%d (значення за замовчуванням, ввід не підтримується системою виконання MATLAB-коду)\n', n);
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
    %% Генерація
    disp('Генеруються вектори та матриці...');
	valueForDataElement = 1;  
    for iIndex = 1:n
	    y1(iIndex,1) = valueForDataElement;
		valueForDataElement = valueForDataElement + 1;  
    end    
	for iIndex = 1:n
        y2(iIndex,1) = valueForDataElement;
	    valueForDataElement = valueForDataElement + 1;
    end    
	for iIndex = 1:n
        for jIndex = 1:n
            Y3(iIndex,jIndex) = valueForDataElement;
			valueForDataElement = valueForDataElement + 1;
        end
    end
    
    disp('Згенеровані вектори та матриці:');
    disp('y1 = '); disp(y1);
    disp('y2 = '); disp(y2);
    disp('Y3 = '); disp(Y3);
end

%% 3. Обчислення x:
disp('Обчислення x...');
x = y2' * y1 * Y3^3 + Y3 * (y2 * y1') * Y3 - Y3^2;

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

printf(' x:\n');
disp(x);
printf('(2025/2026 н.р., KI-309, варіант №30: x = y2'' * y1 * Y3^3 + Y3 * (y2 * y1'') * Y3 - Y3^2)');
