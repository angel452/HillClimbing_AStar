#include <iostream>
#include <vector>
#include <string>
#include "GLFW/glfw3.h"
#include <chrono>
#include <thread>
#include <math.h>
using namespace std;

// ############### VARIABLES GLOBALES - PROGRAMA ####################################
// Expancion en todos los lados
float exp_x[] = { -1,-1, 1, 1, -1, 0, 0, 1 };
float exp_y[] = { -1, 1, -1, 1, 0, -1, 1, 0};

// Esquina superior izquierda
float esqSupIzq_x[] = { 0, 1 };
float esqSupIzq_y[] = { 1, 0 };

// Esquina superior derecha
float esqSupDer_x[] = { 0, 1 };
float esqSupDer_y[] = { -1, 0 };

// Esquina inferior izquierda
float esqInfIzq_x[] = { -1, 0 };
float esqInfIzq_y[] = { 0, 1 };

// Esquina inferior derecha
float esqInfDer_x[] = { -1, 0 };
float esqInfDer_y[] = { 0, -1 };

// ######################### VARIABLES GLOBALES - OPENGL ############################
void drawSquare(float x, float y, float size) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + size, y);
    glVertex2f(x + size, y + size);
    glVertex2f(x, y + size);
    glEnd();
}

void drawLine(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

float size_Square = 0.008f;
float scalarWindow = 60;

// ############################## ESTRUCTURAS #######################################
struct Point{
    float puntos[2];

    Point(){
        for(int i = 0; i < 2; i++)
            puntos[i] = 0;
    }

    Point( float _puntos[] ){
        for (int i = 0; i < 2; i++){
            puntos[i] = _puntos[i];
        }
    }

    void printPointsOfNode(){
        cout << "(" << puntos[0] << "," << puntos[1] << ")";
        //for(int i = 0; i < 2; i++){
        //    cout << puntos[i] << " ";
        //}
    }

    bool isBothOdd(){
        if( (int)puntos[0] % 2 != 0 && (int)puntos[1] % 2 != 0 )
            return true;
        return false;
    }

    void printNode(){
        for(int i = 0; i < 2; i++){
            cout << puntos[i] << " ";
        }
        cout << endl;
    }

    float &operator[]( int posicion ){
        return puntos[posicion];
    }
};

struct Node{
    vector<vector<Point>> data;
    vector<vector<Point>> dataBusquedaProfundidad;

    vector<vector<Point>> dataWithCost;
    vector<Point> dataA_Star;
    //vector<vector<Point>> dataA_Star;

    void inserToData(Point newNodo){
        vector<Point> aux;
        aux.push_back(newNodo);

        data.push_back(aux);
    }

    void printEstructure(){
        cout << "Mostramos los datos en la matriz... ( "<< data.size() << " Puntos en total )"  << endl;
        for(int i = 0; i < data.size();i++){
            for(int j = 0; j < data[i].size(); j++){
                data[i][j].printPointsOfNode();
                cout << " -> ";
            }
            cout << endl;
        }
    }

    void printEstructure2(){
        cout << "Mostramos los datos en la matriz... ( "<< dataWithCost.size() << " Puntos en total )"  << endl;
        for(int i = 0; i < dataWithCost.size();i++){
            for(int j = 0; j < dataWithCost[i].size(); j++){
                dataWithCost[i][j].printPointsOfNode();
                cout << " -> ";
            }
            cout << endl;
        }
    }

    void printAllNodes(){
        // Mostramos los nodos disponibles uno al lado del otro dependiendo de la columna que pertenece
        cout << "Mostramos los datos disponibles... ( "<< data.size() << " Puntos en total )"  << endl;
        float nColumna = data[0][0][0];

        cout << endl << "----------- COLUMNA " << nColumna << " -----------" << endl;
        for(int i = 0; i < data.size();i++){
            if( nColumna != data[i][0][0] ){
                nColumna = data[i][0][0];
                cout << endl << endl;
                cout << "----------- COLUMNA " << nColumna << " -----------" << endl;
            }
            data[i][0].printPointsOfNode();
            cout << "  ";
        }
        cout << endl;
        /*
        for(int i = 0; i < data.size();i++){

            data[i][0].printPointsOfNode();
            cout << endl;
        }
         */
    }

    int searchNode(Point nodo){
        for(int i = 0; i < data.size(); i++){
            if( data[i][0][0] == nodo[0] && data[i][0][1] == nodo[1] )
                return i;
        }
        return -1;
    }

    int searchNode2(Point nodo){
        for(int i = 0; i < dataWithCost.size(); i++){
            if( dataWithCost[i][0][0] == nodo[0] && dataWithCost[i][0][1] == nodo[1] )
                return i;
        }
        return -1;
    }

    bool searchNodeInVector(int posicionNodo, Point nodo){
        for(int i = 0; i < data[posicionNodo].size(); i++){
            if( data[posicionNodo][i][0] == nodo[0] && data[posicionNodo][i][1] == nodo[1] )
                return true;
        }
        return false;
    }

    void createEstructure(){
        // Creamos la lista enlazada para representar las uniones de los nodos
        for(int i = 0; i < data.size(); i++){
            // 1. Expandimos los centros
            if( data[i][0].isBothOdd() ){
                string auxTurn = "sup_izq";
                //cout << "Expancion a: "; data[i][0].printPointsOfNode(); cout << endl;

                // Navegamos en sus 8 vecinos
                for(int z = 0; z < 8; z++){
                    Point aux;
                    aux[0] = data[i][0][0] + exp_x[z];
                    aux[1] = data[i][0][1] + exp_y[z];

                    // Verificamos si el nodo ya esta en la lista
                    int posicionContraria;
                    if( !searchNodeInVector(i, aux) ){
                        data[i].push_back(aux); // Enlace al centro

                        // --> Enlace contrario
                        posicionContraria = searchNode(aux);
                        data[posicionContraria].push_back(data[i][0]);
                    }

                    // 2. Expandimos esquina superior izquierda
                    if( auxTurn == "sup_izq" ) {
                        auxTurn = "sup_der";
                        for(int cont_supIzq = 0; cont_supIzq < 2; cont_supIzq++){
                            Point aux_supIzq;
                            aux_supIzq[0] = data[posicionContraria][0][0] + esqSupIzq_x[cont_supIzq];
                            aux_supIzq[1] = data[posicionContraria][0][1] + esqSupIzq_y[cont_supIzq];

                            // Verificamos si el nodo ya esta en la lista
                            if( !searchNodeInVector(posicionContraria, aux_supIzq) ){
                                data[posicionContraria].push_back(aux_supIzq); // Enlace de esquina superior izquierda

                                // --> Enlace contrario
                                int posicionContraria_SupIzq = searchNode(aux_supIzq);
                                data[posicionContraria_SupIzq].push_back(data[posicionContraria][0]);
                            }
                        }
                    }
                    // 3. Expandimos esquina superior derecha
                    else if( auxTurn == "sup_der" ){
                        auxTurn = "inf_izq";
                        for(int cont_supDer = 0; cont_supDer < 2; cont_supDer++){
                            Point aux_supDer;
                            aux_supDer[0] = data[posicionContraria][0][0] + esqSupDer_x[cont_supDer];
                            aux_supDer[1] = data[posicionContraria][0][1] + esqSupDer_y[cont_supDer];

                            // Verificamos si el nodo ya esta en la lista
                            if( !searchNodeInVector(posicionContraria, aux_supDer) ){
                                data[posicionContraria].push_back(aux_supDer); // Enlace de esquina superior derecha

                                // --> Enlace contrario
                                int posicionContraria_SupDer = searchNode(aux_supDer);
                                data[posicionContraria_SupDer].push_back(data[posicionContraria][0]);
                            }
                        }
                    }
                    // 4. Expandimos esquina inferior izquierda
                    else if( auxTurn == "inf_izq" ){
                        auxTurn = "inf_der";
                        for(int cont_infIzq = 0; cont_infIzq < 2; cont_infIzq++){
                            Point aux_infIzq;
                            aux_infIzq[0] = data[posicionContraria][0][0] + esqInfIzq_x[cont_infIzq];
                            aux_infIzq[1] = data[posicionContraria][0][1] + esqInfIzq_y[cont_infIzq];

                            // Verificamos si el nodo ya esta en la lista
                            if( !searchNodeInVector(posicionContraria, aux_infIzq) ){
                                data[posicionContraria].push_back(aux_infIzq); // Enlace de esquina inferior izquierda

                                // --> Enlace contrario
                                int posicionContraria_InfIzq = searchNode(aux_infIzq);
                                data[posicionContraria_InfIzq].push_back(data[posicionContraria][0]);
                            }
                        }
                    }
                    // 5. Expandimos esquina inferior derecha
                    else if( auxTurn == "inf_der" ){
                        auxTurn = "none";
                        for(int cont_infDer = 0; cont_infDer < 2; cont_infDer++){
                            Point aux_infDer;
                            aux_infDer[0] = data[posicionContraria][0][0] + esqInfDer_x[cont_infDer];
                            aux_infDer[1] = data[posicionContraria][0][1] + esqInfDer_y[cont_infDer];

                            // Verificamos si el nodo ya esta en la lista
                            if( !searchNodeInVector(posicionContraria, aux_infDer) ){
                                data[posicionContraria].push_back(aux_infDer); // Enlace de esquina inferior derecha

                                // --> Enlace contrario
                                int posicionContraria_InfDer = searchNode(aux_infDer);
                                data[posicionContraria_InfDer].push_back(data[posicionContraria][0]);
                            }
                        }
                    }
                }
            }
        }
        //printEstructure();
    }

    bool compairTwoPoints(Point a, Point b){
        if( a[0] == b[0] && a[1] == b[1] )
            return true;
        else
            return false;
    }

    void deleteNodos(int porcentaje, int nNodosLados){
        int nNodosTotal = nNodosLados * nNodosLados;
        int nNodosEliminar = (nNodosTotal * porcentaje) / 100;

        cout << "--> Cantidad de nodos a eliminar: " << nNodosEliminar << endl;

        for(int i = 0; i < nNodosEliminar; i++){
            int pos = rand() % nNodosTotal;
            //cout << "Posicion a eliminar -> " << pos << ": "; data[pos][0].printNode(); cout << endl;

            //Eliminar sus conexiones antes de eliminar el nodo
            for(int j = 1; j < data[pos].size(); j++){ // recorre las conexiones del nodo[pos]
                int posContraria = searchNode(data[pos][j]); // Buscamos la posicion del nodo contrario --> posC = 6
                for(int k = 1; k < data[posContraria].size(); k++){
                    if( compairTwoPoints(data[posContraria][k], data[pos][0]) ){
                        data[posContraria].erase(data[posContraria].begin() + k);
                    }
                }
            }

            // print ...
            //cout << "################ test ################" << endl;
            //printEstructure();
            //cout << "############ fin test ############" << endl;
            data.erase(data.begin() + pos);

            // Actualizamos la cantidad de nodos
            nNodosTotal--;
        }

    }

    float distanciaEuclidiana(Point a, Point b){
        float x = pow( (a[0] - b[0]), 2 );
        float y = pow( (a[1] - b[1]), 2 );
        return sqrt( x + y );
    }

    void busquedaProfundidad( Point inicio, Point fin ){
        int ptr_posInicio = searchNode(inicio); // Buscamos la posicion del nodo inicial
        int ptr_posFin = searchNode(fin); // Buscamos la posicion del nodo final

        // Matriz de n x 1 (n = cantidad de nodos) donde todos los datos son false
        bool visitados[data.size()];
        for(int i = 0; i < data.size(); i++)
            visitados[i] = false;

        // --> Primera insercion
        vector<Point> test;
        test.push_back(data[ptr_posInicio][0]);
        dataBusquedaProfundidad.push_back(test);

        while(ptr_posInicio != ptr_posFin){
            // --> 1. Buscamos la posicion del puntero inicio
            ptr_posInicio = searchNode(inicio);

            // --> FLAG
            if( ptr_posInicio == ptr_posFin ){
                cout  << "----------------------------------------" << endl << endl;
                cout << "Llegamos al nodo final" << endl;

                // --> 7. Imprimimos el camino
                cout << "Camino: ";
                for(int i = 0; i < dataBusquedaProfundidad[0].size(); i++){
                    cout << "(" << dataBusquedaProfundidad[0][i][0] << "," << dataBusquedaProfundidad[0][i][1] << ") ";
                }
                cout << endl;
                break;
            }

            // --> 2. Marcamos el nodo como visitado
            visitados[ptr_posInicio] = true;

            for(int i = 1; i < data[ptr_posInicio].size(); i++){
                // --> 3. Verificamos si su hijo ya fue visitado
                int ptr_posHijo = searchNode(data[ptr_posInicio][i]);

                if( visitados[ptr_posHijo] == false ){ // hijo no visitado
                    // --> 4.1) Añado en un vector al nodo hijo
                    vector<Point> vec_Hijo;
                    vec_Hijo.push_back(data[ptr_posHijo][0]);

                    // --> 4.2) Despues de dicho nodo, añado todos los nodos de la primera posicion
                    for(int j = 0; j < dataBusquedaProfundidad[0].size(); j++){
                        vec_Hijo.push_back(dataBusquedaProfundidad[0][j]);
                    }

                    // --> 4.3) Añado el vector hijo en dataBusquedaProfundidad despues del primero dato
                    dataBusquedaProfundidad.insert(dataBusquedaProfundidad.begin() + 1, vec_Hijo);
                }
            }
            // --> 5. Eliminamos el primer dato de dataBusquedaProfundidad
            dataBusquedaProfundidad.erase(dataBusquedaProfundidad.begin());

            // --> 6. Actualizamos el inicio al primero nodo
            inicio = dataBusquedaProfundidad[0][0];
        }
        // std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void modificarEstructure(){
        // --> Modificamos data para que cada conexcion entre nodos tenga un peso aleatorio entre 1 y 10
        for(int i = 0; i < data.size(); i++){
            // --> Añadimos el primer nodo a dataWithCost
            vector<Point> auxNodoInicio;
            auxNodoInicio.push_back(data[i][0]);
            dataWithCost.push_back(auxNodoInicio);

            for(int j = 1; j < data[i].size(); j++){
                // --> Verificamos si ya hay una conexion entre los nodos
                int pos = searchNode2(data[i][j]);
                if( pos == -1){
                    // --> No hay conexion
                    // Creamos un peso aleatorio
                    float peso[2];
                    peso[0] = rand() % 10 + 1;
                    peso[1] = peso[0];
                    Point auxPeso(peso);

                    // Añadimos a la nueva estructura un peso y el nodo de data
                    dataWithCost[i].push_back(auxPeso);
                    dataWithCost[i].push_back(data[i][j]);
                }
                else{
                    // --> Hay conexion
                    // Buscamos en esa posicion el primer nodo
                    for(int z = 2; z < dataWithCost[pos].size(); z = z + 2){
                        if( compairTwoPoints(dataWithCost[pos][z], data[i][0]) ){
                            // --> Pusheamos el peso que es una posicion antes del nodo encontrodo
                            dataWithCost[i].push_back(dataWithCost[pos][z-1]);
                            dataWithCost[i].push_back(data[i][j]);
                            break;
                        }
                    }
                }
            }
        }

        cout << endl <<  "Data modificada: " << endl;
        //printEstructure2();
        cout << endl;
    }

    void A_Star(Point inicio, Point final, bool visitados[]){
        // --> Empezamos el algoritmo
        int ptr_posInicio = searchNode2(inicio); // Buscamos la posicion del nodo inicial
        int ptr_posFin = searchNode2(final); // Buscamos la posicion del nodo final

        // --> Primera insercion
        dataA_Star.push_back(dataWithCost[ptr_posInicio][0]);

        //vector<Point> test;
        //test.push_back(dataWithCost[ptr_posInicio][0]);
        //dataA_Star.push_back(test);

        while(ptr_posInicio != ptr_posFin){
            ptr_posInicio = searchNode2(inicio);

            // --> FLAG
            if( ptr_posInicio == ptr_posFin ){
                cout  << "----------------------------------------" << endl << endl;
                cout << "Llegamos al nodo final" << endl;

                // --> 7. Imprimimos el camino
                cout << "Camino: ";
                for(int i = 0; i < dataA_Star.size(); i++){
                    cout << "(" << dataA_Star[i][0] << "," << dataA_Star[i][1] << ") ";
                }
                cout << endl;
                break;
            }

            // --> 2. Marcamos el nodo como visitado
            visitados[ptr_posInicio] = true;

            // --> 3. Buscamos el nodo con menor peso y menor distancia al final
            float distancia = 10000;
            int posMenor = -1;

            for(int i = 2; i < dataWithCost[ptr_posInicio].size(); i = i + 2){

                // --> Verificamos si el nodo actual ya fue visitado
                int nodoAux = searchNode2(dataWithCost[ptr_posInicio][i]);
                if( visitados[nodoAux] == false ){
                    //cout << endl << "El nodo (" << dataWithCost[ptr_posInicio][i][0] << "," << dataWithCost[ptr_posInicio][i][1] << ") no fue visitado" << endl;
                    // --> Calculamos la distancia
                    float distanciaAux = distanciaEuclidiana(dataWithCost[ptr_posInicio][i], final);
                    
                    // *** Para aplicar la logica del A*, tenemos que sumar tanto la distancia euclidiana y el costo del camino
                    //distanciaAux = distanciaAux + dataWithCost[ptr_posInicio][i-1][0];
                    // *** Si queremos usar la logica del Hill Climbing, solo usamos la distancia euclidiana
                    distanciaAux = distanciaAux + 0;

                    // Guardamos al menor
                    if( distanciaAux < distancia ){
                        distancia = distanciaAux;
                        posMenor = i;
                    }
                }
                else{
                    //cout << "el nodo (" << dataWithCost[ptr_posInicio][i][0] << "," << dataWithCost[ptr_posInicio][i][1] << ") ya fue visitado" << endl;
                }
            }

            // --> Caso donde no hay mas nodos por visitar
            if(posMenor == -1 ){
                cout << "Callejon sin salida - Empezando el backtracking" << endl;

                // --> Actualizamos el inicio al penultimo nodo de dataA_Star
                //cout << "el inicio ahora sera " << dataA_Star[dataA_Star.size()-2][0] << "," << dataA_Star[dataA_Star.size()-2][1] << endl;
                inicio = dataA_Star[dataA_Star.size()-2];

                // --> Eliminamos los 2 ultimos nodos
                dataA_Star.pop_back();
                dataA_Star.pop_back();

                //std::this_thread::sleep_for(std::chrono::seconds(1));

                // --> Recurcion
                A_Star(inicio, final, visitados);
                break;
            }

            // --> 4. Añadimos el nodo con menor peso y menor distancia al final
            dataA_Star.push_back(dataWithCost[ptr_posInicio][posMenor]);
            //cout << "Nodo añadido: (" << dataWithCost[ptr_posInicio][posMenor][0] << "," << dataWithCost[ptr_posInicio][posMenor][1] << ")" << endl;

            // --> 5. Actualizamos el inicio al nodo con menor peso y menor distancia al final
            inicio = dataWithCost[ptr_posInicio][posMenor];

        }
    }

    float convertEscalarWindow(float num){
        return (num / scalarWindow)+0.1;
    }

    int printAllNodes_OpenGL(){
        if (!glfwInit()) {
            return -1;
        }

        GLFWwindow* window = glfwCreateWindow(800, 800, "Total nodos creados (100x100)", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);

            // Recorremos todos los nodos en la primera posicion de cada vector
            for(int i = 0; i < data.size(); i++){
                // Para graficar los cuadrados
                float posX_Square, posY_Square;
                posX_Square = convertEscalarWindow(data[i][0][0]);
                posY_Square = convertEscalarWindow(data[i][0][1]);


                // Dibujar varios cuadrados
                glColor3f(0.0f, 1.0f, 0.0f); // Color verde
                drawSquare(posX_Square, posY_Square, size_Square);

                // Dibujar lineas
                float posX1_Line, posY1_Line, posX2_Line, posY2_Line;
                posX1_Line = convertEscalarWindow(data[i][0][0]) + (size_Square / 2);
                posY1_Line = convertEscalarWindow(data[i][0][1]) + (size_Square / 2);
                for(int j = 1; j < data[i].size(); j++){
                    posX2_Line = convertEscalarWindow(data[i][j][0]) + (size_Square / 2);
                    posY2_Line = convertEscalarWindow(data[i][j][1]) + (size_Square / 2);

                    // Cambiamos al color celeste
                    drawLine(posX1_Line, posY1_Line, posX2_Line, posY2_Line);
                }
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
        return 0;
    }

    int printPath_OpenGL(){
        if (!glfwInit()) {
            return -1;
        }

        GLFWwindow* window = glfwCreateWindow(800, 800, "Camino", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);

            // ########### GRAFICAMOS LOS NODOS VERDES ############
            for(int i = 0; i < data.size(); i++){
                // Para graficar los cuadrados
                float posX_Square, posY_Square;
                posX_Square = convertEscalarWindow(data[i][0][0]);
                posY_Square = convertEscalarWindow(data[i][0][1]);


                // Dibujar varios cuadrados
                glColor3f(0.0f, 1.0f, 0.0f); // Color verde
                drawSquare(posX_Square, posY_Square, size_Square);

                // Dibujar lineas
                float posX1_Line, posY1_Line, posX2_Line, posY2_Line;
                posX1_Line = convertEscalarWindow(data[i][0][0]) + (size_Square / 2);
                posY1_Line = convertEscalarWindow(data[i][0][1]) + (size_Square / 2);
                for(int j = 1; j < data[i].size(); j++){
                    posX2_Line = convertEscalarWindow(data[i][j][0]) + (size_Square / 2);
                    posY2_Line = convertEscalarWindow(data[i][j][1]) + (size_Square / 2);

                    drawLine(posX1_Line, posY1_Line, posX2_Line, posY2_Line);
                }
            }

            // ########### GRAFICAMOS EL CAMINO ROJO ############
            // Recorremos todos los nodos de la primera posicion de dataBusquedaProfundidad
            for(int i = 0; i < dataBusquedaProfundidad[0].size(); i++){ // Para graficar los cuadrados
                float posX_Square, posY_Square;
                posX_Square = convertEscalarWindow(dataBusquedaProfundidad[0][i][0]);
                posY_Square = convertEscalarWindow(dataBusquedaProfundidad[0][i][1]);

                // Dibujar varios cuadrados de color rojo
                glColor3f(1.0f, 0.0f, 0.0f); // Color rojo
                drawSquare(posX_Square, posY_Square, size_Square);
            }

            // Para graficar las lineas
            float posX1_Line, posY1_Line, posX2_Line, posY2_Line;
            // --> inicializamos el primer punto de la linea
            posX1_Line = convertEscalarWindow(dataBusquedaProfundidad[0][0][0]) + (size_Square/2);
            posY1_Line = convertEscalarWindow(dataBusquedaProfundidad[0][0][1]) + (size_Square/2);
            for (int i = 1; i < dataBusquedaProfundidad[0].size(); i++){
                posX2_Line = convertEscalarWindow(dataBusquedaProfundidad[0][i][0]) + (size_Square/2);
                posY2_Line = convertEscalarWindow(dataBusquedaProfundidad[0][i][1]) + (size_Square/2);

                // Dibujar varias lineas de color rojo
                glColor3f(1.0f, 0.0f, 0.0f); // Color rojo
                drawLine(posX1_Line, posY1_Line, posX2_Line, posY2_Line);

                // --> Actualizamos el primer punto de la linea
                posX1_Line = posX2_Line;
                posY1_Line = posY2_Line;
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
        return 0;
    }

    int printPath2_OpenGL(){
        if (!glfwInit()) {
            return -1;
        }

        GLFWwindow* window = glfwCreateWindow(800, 800, "Camino", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);

            // ########### GRAFICAMOS LOS NODOS VERDES ############
            for(int i = 0; i < data.size(); i++){
                // Para graficar los cuadrados
                float posX_Square, posY_Square;
                posX_Square = convertEscalarWindow(data[i][0][0]);
                posY_Square = convertEscalarWindow(data[i][0][1]);


                // Dibujar varios cuadrados
                glColor3f(0.0f, 1.0f, 0.0f); // Color verde
                drawSquare(posX_Square, posY_Square, size_Square);

                // Dibujar lineas
                float posX1_Line, posY1_Line, posX2_Line, posY2_Line;
                posX1_Line = convertEscalarWindow(data[i][0][0]) + (size_Square / 2);
                posY1_Line = convertEscalarWindow(data[i][0][1]) + (size_Square / 2);
                for(int j = 1; j < data[i].size(); j++){
                    posX2_Line = convertEscalarWindow(data[i][j][0]) + (size_Square / 2);
                    posY2_Line = convertEscalarWindow(data[i][j][1]) + (size_Square / 2);

                    drawLine(posX1_Line, posY1_Line, posX2_Line, posY2_Line);
                }
            }

            // ########### GRAFICAMOS EL CAMINO ROJO ############
            // Recorremos todos los nodos de dataA_Star
            for(int i = 0; i < dataA_Star.size(); i++){ // Para graficar los cuadrados
                float posX_Square, posY_Square;
                posX_Square = convertEscalarWindow(dataA_Star[i][0]);
                posY_Square = convertEscalarWindow(dataA_Star[i][1]);

                // Dibujar varios cuadrados de color rojo
                glColor3f(1.0f, 0.0f, 0.0f); // Color rojo
                drawSquare(posX_Square, posY_Square, size_Square);
            }

            // Para graficar las lineas
            float posX1_Line, posY1_Line, posX2_Line, posY2_Line;
            // --> inicializamos el primer punto de la linea
            posX1_Line = convertEscalarWindow(dataA_Star[0][0]) + (size_Square/2);
            posY1_Line = convertEscalarWindow(dataA_Star[0][1]) + (size_Square/2);
            for (int i = 1; i < dataA_Star.size(); i++){
                posX2_Line = convertEscalarWindow(dataA_Star[i][0]) + (size_Square/2);
                posY2_Line = convertEscalarWindow(dataA_Star[i][1]) + (size_Square/2);

                // Dibujar varias lineas de color rojo
                glColor3f(1.0f, 0.0f, 0.0f); // Color rojo
                drawLine(posX1_Line, posY1_Line, posX2_Line, posY2_Line);

                // --> Actualizamos el primer punto de la linea
                posX1_Line = posX2_Line;
                posY1_Line = posY2_Line;
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
        return 0;
    }

    int printNode_OpenGL(Point node){
        if (!glfwInit()) {
            return -1;
        }

        GLFWwindow* window = glfwCreateWindow(800, 800, "Camino", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);

            // ########### GRAFICAMOS LOS NODOS VERDES ############
            for(int i = 0; i < data.size(); i++){
                // Para graficar los cuadrados
                float posX_Square, posY_Square;
                posX_Square = convertEscalarWindow(data[i][0][0]);
                posY_Square = convertEscalarWindow(data[i][0][1]);


                // Dibujar varios cuadrados
                glColor3f(0.0f, 1.0f, 0.0f); // Color verde
                drawSquare(posX_Square, posY_Square, size_Square);

                // Dibujar lineas
                float posX1_Line, posY1_Line, posX2_Line, posY2_Line;
                posX1_Line = convertEscalarWindow(data[i][0][0]) + (size_Square / 2);
                posY1_Line = convertEscalarWindow(data[i][0][1]) + (size_Square / 2);
                for(int j = 1; j < data[i].size(); j++){
                    posX2_Line = convertEscalarWindow(data[i][j][0]) + (size_Square / 2);
                    posY2_Line = convertEscalarWindow(data[i][j][1]) + (size_Square / 2);

                    drawLine(posX1_Line, posY1_Line, posX2_Line, posY2_Line);
                }
            }

            // ########### GRAFICAMOS LOS NODOS INICIO O FINAL ############
            float posX_Square, posY_Square;
            posX_Square = convertEscalarWindow(node[0]);
            posY_Square = convertEscalarWindow(node[1]);

            // Dibujar cuadrados de color rojo
            glColor3f(1.0f, 0.0f, 0.0f); // Color rojo
            drawSquare(posX_Square, posY_Square, size_Square);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();
        return 0;
    }
};

// ############################## MAIN ##############################
int main(){
    // OJO: Solo numeros impares para nNodosLado: {5,7,9,...}
    int nNodosLado = 51; // 100 x 100

    /*
    /*
     * Ejemplos
     * - 5 x 5 (con 50% de eliminados)
     *    - ini: 0,3
     *    - fin: 2,1
     *
     * - 5 x 5 (con 10% de eliminados)
     *    - ini: 0,0
     *    - fin: 4,4
     *
     * - 21 x 21 (con 50% de eliminados)
     *    - ini: 5,10
     *    - fin: (20,9) o (6,9)
     *
     * - 51 x 51 (con 30% de eliminados)
     *    - ini: 0,0
     *    - fin: 47,50
     */

    // DIBUJANDO EL ARBOL
    Node arbol;

    // --> Creando todos los puntos
    for(int i = 0; i < nNodosLado; i++){
        for(int j = 0; j < nNodosLado; j++){
            float dataAux[2];
            dataAux[0] = i;
            dataAux[1] = j;
            Point aux(dataAux);
            arbol.inserToData(aux);
        }
    }

    arbol.createEstructure();

    int respuestaMenu;
    cout << "------------------- BUSQUEDA POR PROFUNDIDAD -----------------" << endl;
    cout << "Ver los nodos del arbol: ........................... 1" << endl;
    cout << "Iniciar con la busqueda: ........................... 2" << endl;
    cout << "Salir: ............................................. 0" << endl;
    cout << "Ingrese su opcion: "; cin >> respuestaMenu;

    if( respuestaMenu == 1 ){
        cout << "---------------------------------------------------------------" << endl;
        //arbol.printEstructure();
        arbol.printAllNodes();
        arbol.printAllNodes_OpenGL();

    }
    else if( respuestaMenu == 0 ){
        return 0;
    }

    cout << endl << "###############################################################" << endl;
    cout << "Ingrese la cantidad de nodos a eliminar (%): " << endl;
    int porcentaje;
    cin >> porcentaje;

    arbol.deleteNodos(porcentaje, nNodosLado);
    cout << "###############################################################" << endl << endl;

    //arbol.printEstructure();
    arbol.printAllNodes();
    arbol.printAllNodes_OpenGL();

    cout << endl << "----------------------------------------------------------" << endl;
    float nodoInicio[2], nodoFinal[2];
    cout << " --> Comenzando busqueda " << endl;
    cout << "Seleccione el nodo de INICIO: (coordenada X)" << endl;
    cin >> nodoInicio[0];
    cout << "Seleccione el nodo de INICIO: (coordenada Y)" << endl;
    cin >> nodoInicio[1];

    // --> Imprimimos nodo seleccionado
    Point inicio(nodoInicio);
    arbol.printNode_OpenGL(inicio);

    cout << "Seleccione el nodo de FINAL: (coordenada X)" << endl;
    cin >> nodoFinal[0];
    cout << "Seleccione el nodo de FINAL: (coordenada Y)" << endl;
    cin >> nodoFinal[1];

    // --> Imprimimos nodo seleccionado
    Point final(nodoFinal);
    arbol.printNode_OpenGL(final);

    //arbol.busquedaProfundidad(inicio, final);
    //arbol.printPath_OpenGL();

    arbol.modificarEstructure();
    // Matriz de n x 1 (n = cantidad de nodos) donde todos los datos son false
    bool visitados[arbol.dataWithCost.size()];
    for(int i = 0; i < arbol.dataWithCost.size(); i++)
        visitados[i] = false;

    arbol.A_Star(inicio, final, visitados);
    arbol.printPath2_OpenGL();

    return 0;
}
