Тестовое задание на стажировку в C3D Labs.</br>
Была произведена модификация файлов triangle.cpp и basicsgeomc.cpp из тестового примера C3D Vision.</br>

MainTasks.cpp - содержит все задания</br>
AnotherVersionFirstTask.cpp - второй вариант создания элементарных тел из первого задания</br>

1. Создать два элементарных тела на выбор.
Были выбраны тор и цилиндр.
Создано две версии отображения: генерация через ElementarySolid (a) и GeometryRep (b).</br>
a.
<img width="1285" height="728" alt="1 2_Тор" src="https://github.com/user-attachments/assets/5b44e835-2660-4699-ab46-391422b8290e" /></br>
<img width="1287" height="729" alt="1 2_Цилиндр" src="https://github.com/user-attachments/assets/a8744b82-1693-47ee-a16a-11a646cb7aab" /></br>
b.
<img width="1285" height="726" alt="1_Тор" src="https://github.com/user-attachments/assets/1a206773-3ef2-4e0d-b6e2-e51c6e0432e4" /></br>
<img width="1283" height="730" alt="1_Цилиндр" src="https://github.com/user-attachments/assets/bd959aef-ea68-4ca9-b451-68f655c5a21c" /></br>

3. Тело, полученное в результате выполнения булевой операции.
Два варианта: вычитание (a) и пересечение (b).
Рядом с результатом операции изначальные объекты и их положение.
a.</br>
<img width="1286" height="728" alt="2 2_Булево_Вычитание" src="https://github.com/user-attachments/assets/835f3d36-8f9b-4220-8380-6dc6b464e100" /></br>
b.</br>
<img width="1285" height="730" alt="2 2_Булево_Пересечение" src="https://github.com/user-attachments/assets/efb0ddb2-fb31-4507-ac52-fbdf0322ef43" /></br>

5. Тело, полученное в результате выполнения операции локальной модификации на выбор.</br>
Выбрано скругление.</br>
<img width="1287" height="728" alt="3_Скругление" src="https://github.com/user-attachments/assets/d849c3f9-0201-4091-899b-04086286d006" /></br>

6. Тела заметания по цилиндрической винтовой линии вращения ромбов с углами 30
и 40 градусов. При построении ромба использовать Solver.</br>
Ромб 30 градусов (a) и 40 градусов (b).</br>
a.
<img width="1285" height="725" alt="4 1_Заметание_30" src="https://github.com/user-attachments/assets/4c4e5031-d8a5-4a6e-bc69-10eb2117860f" /></br>
b.
<img width="1285" height="726" alt="4 3_Заметание_40" src="https://github.com/user-attachments/assets/91588a0a-bea3-4e13-86c5-c508bcd7792d" /></br>

Отличие между 30 и 40 градусами.</br>
<img width="692" height="228" alt="4 2_Ромб_30" src="https://github.com/user-attachments/assets/fd593ae2-10bf-4d00-bbc7-872c506fcbfa" /></br>
<img width="587" height="427" alt="4 4_Ромб_40" src="https://github.com/user-attachments/assets/015eb813-053a-4600-b256-9099f94628ef" /></br>
Оно может быть не так сильно заметно, поэтому есть еще вариант на 60 градусов, который лучше отображает зависимость от угла.</br>
<img width="547" height="375" alt="4 5_Ромб_60" src="https://github.com/user-attachments/assets/68eba94f-c7f2-4494-9e35-41c10973c157" /></br>

ПРИМЕЧАНИЕ:</br>
Углы ромба и стороны захардкожены, так как не получилось применить операции Solver
именно для выравнивания всех сторон и добавления нужного угла.</br>
Код этих операций все равно содержится в файле с комментариями.</br>

5. Полигональная модель тела, полученная вычисления триангуляции.</br>
<img width="1284" height="726" alt="5_Триангуляция" src="https://github.com/user-attachments/assets/b72cedf3-457d-4ebd-8750-c6b00d67ea5e" /></br>

6. Полигональная модель тела, полученная с помощью операции ремешинга.</br>
<img width="1282" height="725" alt="6_Ремешинг" src="https://github.com/user-attachments/assets/7007915e-fb31-49c5-a7f8-ba4891981e86" /></br>

К сожалению, не удалось настроить нормальное отображение для объекта ремешинга.
