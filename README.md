# ntar
nomiram tar - это попытка написания архиватора аналогичного tar

Справка по формату файла .ntar:
      file .ntar format (ntar1.0)
                FILE                                   FOLDER
bytes  10        1     1     50   2    SIZE          1     1     50
      META|... |TYPE|RECLVL|NAME|SIZE|CONTENT|...  |TYPE|RECLVL|NAME|...
RECLVL-level of recursion(вложенность)
