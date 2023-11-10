***Instrucciones de compilacion***
---
- Insertar los comandos:
- cmake CMakeLists.txt
- make
- ./main

***Consideraciones***
---
Comentar las lineas 475 o 477, dependiendo de que tipo de algoritmo queremos ejecutar.
- Para aplicar la logica del A*, tenemos que sumar tanto la distancia euclidiana y el costo del camino
- Si queremos usar la logica del Hill Climbing, solo usamos la distancia euclidiana