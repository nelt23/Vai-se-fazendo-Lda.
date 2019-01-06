//
//  main.c
//  Vai-se fazendo Lda.
//
//  Created by Nuno Martins on 05/12/2018.
//  Copyright © 2018 Nuno Martins. All rights reserved.
//

// *** COMENTARIOS ***

// 2. So no na saida é que guarda no file Stock
// 3. No buildCar começar por ler o ficheiro binario

// NOTA SE O FICHEIRO tiver 1 ESPAÇO A MAIS NO FINAL O MATERIAL QUE ESTA ANTERIOR FICA COM MAIS 1
// ***IMPORTATE os ficheiros nao podem espaço antes nem depois se ao vai alterar Tudo
// Erro1 as vezes quando guarda no ficherio processing o 1 vetor fica desconfiguardo nas strings mas isso so acontece quando temos uma grande quantidade de carros +5
// Se o ficheiro tiver + que 9 carros e guardarmos

// NOTAS: NAO SEI SE HAVIA ALGUMA FORMA MAIS SIMPLES PARA CALCULAR O TAMANHO DO ARRAY DO PROCESSING SEM CRIAR UMA FUNÇAO PARA ABRIR O FICHEIRO E CONTAR O NUMERO DE LINHAS LA PRESENTES

// RefillStock.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

// *** STRUCT *** //

// Estrutura do material
typedef struct {
      // Os ID correspondem aos indices logo o indice 0 nunca contem nada
    int Quantity[7];
    
} StockStruct;
StockStruct Stock;

// Se a estação esta a trabalhar ou nao (1 -> ocupada/0  -> Livre)
typedef struct {
    bool StationWork;
} StationBoolStruct;

// Estrutura com os dados das estaçoes
typedef struct {
    
    int ID;
    char Ops[3];
    int DurationOps[3];
    double CostPerMin;
    int TotalDurationOps;
    StationBoolStruct StationBool;
    
}  StationStruct;
StationStruct StationsArray[5];

// Estrutura com os dados dos carros
typedef struct {
    
    char Type[9];
    char State[11];
    char Local;
    int TimeLeft;
    char OperationsLeft[4];
    
} ProcessingStruct;
ProcessingStruct *Processing;

// *** CONSTANTES *** //

FILE * Filetxt;

// A primeira posição (0) nunca se utiliza
const char TypeNames[7][9] = {"","Ferro   ","Aluminio","Vidro   ","Plastico","Borracha","Aço     "};
const float Price[7] = {0,1.40,7.80,12.50,2.20,1.50,3.30};
const int CarsMaterials[6][7] = {{-1,-1,-1,-1,-1,-1,-1},{-1,2,0,1,3,0,0},{-1,0,3,2,0,0,2},{-1,4,0,1,3,2,0},{-1,0,1,2,3,1,3},{-1,2,2,3,0,0,2}};
const char CarsNames[6][9] = {"","Renault","Mercedes","Honda","Ford","Ferrari"};
const char CarsOperations[6][4] = {"","BAD","AEC","BCE","ABD","DCE"};
const char CarsStates[3][11] = {"Finished","Processing","Waiting"};

int TotalOfProcessingStruct = 0;
int IndexOfProcessingStationAndOps[2] = {-1,-1};
bool ReadStockFile = 0;

// *** FUNCOES EXTRAS *** //

void Menu(void);
void FactoryState(void);
void Stats(void);


void QuantityStockForStruct(StockStruct *S) {
    
    int ID;
    int CountFirstChar = 0;
    
    // Obter apenas o ID de cada linha, começa com count = 0 e se estiver 0 é porque estamos no 1 elemento da linha sempre que encontra \n o count volta a 0 e voltamos outra vez para o 1 elemento da linha
    
    // Ex: [0,5,4,3,0,0,19]
    // Id     1 2 3 4 5 6
    
    // Enquanto for diferente de EOF
    while (!feof(Filetxt)) {
        
        if (CountFirstChar == 0) {
            fscanf(Filetxt, "%d",&ID);
            
            int * QuantitySum = S->Quantity+ID;
            ++(*QuantitySum);
            
            CountFirstChar = 1;
        } else if (fgetc(Filetxt) == '\n') {
            CountFirstChar = 0;
        }
    }
}


void ReadStockFileForStruct(StockStruct *S) {
    
    Filetxt = fopen("Stock.txt", "r");

    if (Filetxt == NULL) {
        printf("Não foi possivel encontrar/abrir esse ficheiro ERRO 1");
        exit(1);
    }
    
    QuantityStockForStruct(S);
    // O ficheiro ja foi lido
    ReadStockFile = 1;
    
    fclose(Filetxt);
}


// Como é um vetor nao precisa de apontador
void ReadStationBinaryFileForStruct() {
    
    Filetxt = fopen("Stations.dat", "rb");
    
    if (Filetxt == NULL) {
        printf("Não foi possivel encontrar/abrir esse ficheiro ERRO 2");
        exit(2);
    }
    
    // Como so sao 5 estaçoes
    while (fread(StationsArray, sizeof(StationsArray), 5, Filetxt) == 5);

    fclose(Filetxt);
}


void ReadProcessingFileForStruct(ProcessingStruct * P) {
    
    Filetxt = fopen("Processing.txt", "r");
    
    if (Filetxt == NULL) {
        printf("Não foi possivel encontrar/abrir esse ficheiro ERRO 3");
        exit(3);
    }
    
    int j=0;
    
    char NumberSlash;
    
    while (fscanf(Filetxt, "%s %s %c %d %s",(P+j)->Type,(P+j)->State,&NumberSlash,&(P+j)->TimeLeft,(P+j)->OperationsLeft) != EOF) {

        // Converter de caracter para decimal
        // 49-> 1 // 53 -> 5
        if ((NumberSlash>=49) && (NumberSlash<=53)) {
            (P+j)->Local = NumberSlash-48;
        } else {
            (P+j)->Local = '-';
        }
        
        j++;
    }
    
    // Numero de carros na estrutura
    TotalOfProcessingStruct = j;
    
    fclose(Filetxt);
}

// Calcular o numero de carros no ficheiro
int ProcessingFileLengthOfArrayStruct() {
    
    int CountLines = 0;
    char c;
    
    Filetxt = fopen("Processing.txt", "r");
    
    if (Filetxt == NULL) {
        printf("Não foi possivel encontrar/abrir esse ficheiro ERRO 4");
        exit(4);
    }
    
    // Contar o numero de linhas no Processing que correspondem ao tamanho do array
    while (!feof(Filetxt)) {
        c = fgetc(Filetxt);
        
        if (c == '\n') {
            CountLines++;
        }
    }
    
    // Adicionar mais 1 para a contagem das linhas ficar correta
    CountLines++;
    
    fclose(Filetxt);
    
    // -> APENAS SERVE PRA VER SE O NUMERO DE CARROS NO FILE PROCESSING
    printf("Tamanho do array de estruturas  -> %d\n",CountLines);
    
    return CountLines;
}


// Comparar o stock que temos com os materiais necessarios para construir o carro
int CheckStockIsAvailable(StockStruct *Stock, int OptionAdmin) {
    
    // Se o valor for >=0 é por que pode
    
    // inicializar tudo a 0
    // Serve para retirar do stock os materiais que usamos
    int ArrayUseStock[7] = {};
    
    int i,Available = 1;
    
    for(i=1;i<7;i++) {
        
        int StockQuantity = Stock->Quantity[i];
        int Diference = StockQuantity-CarsMaterials[OptionAdmin][i];
        
        // Material Disponivel - // Numero do carro // materiais necessarios < 0
        if (Diference < 0) {
            
            // Mostra os materiais que ainda faltam
            printf("Missing: %d %s\n",abs(Diference),TypeNames[i]);
            Available = 0;
        } else {
            ArrayUseStock[i] = CarsMaterials[OptionAdmin][i];
        }
    }
    
    // So se for 1 é que retira os materiais da estrutura stock
    if (Available) {
        
        // Serve para retirar os materiais da Stock
        for(i=1;i<7;i++) {
            // Retirar da Struct os materiais utilizados
            Stock->Quantity[i] = Stock->Quantity[i]-ArrayUseStock[i];
            
        }
        
        // Existem
        return 1;
    } else {
        // Faltam
        return 0;
    }
    
}


// Isto é pra dizer quais é que sao as estaçoes livres
void StationsAvailable(ProcessingStruct *P) {
    int i,j;
    
    // Inicializar tudo a 0
    for(j=0;j<5;j++) {
        StationsArray[j].StationBool.StationWork = 0;
    }
    
    for(i=0;i<TotalOfProcessingStruct;i++) {

        // Ou seja quer dizer que existe alguma estação ocupada
        if ((P+i)->Local != '-') {
            // Nao esquecer que no Processing começa no index 1 e no StationArray começa no index 0
            // Colocar a 1 estaçao ocupada
            StationsArray[((P+i)->Local)-1].StationBool.StationWork = 1;
        }
    }
}


// Das estaçoes livres aquela que faz a operação mais barata
void StationWithOperationFreeAndCheap(char IDOperation) {
    
    int StationIndex = -1;
    int FirstOpsIndex = -1;
    int GetFistValue = 1;
    int i;
    
    // Agora das que estao livres (0) verificar se existe alguma maquina com a operação que queremos
    for(i=0;i<5;i++) {

        // A maquina está livre
        if (StationsArray[i].StationBool.StationWork == 0) {
            
            // Da nos o adress do 1 caracter comum nos 2 casos (neste caso é a operaçao necessaria)
            char *FirstOpsAdress = strchr(StationsArray[i].Ops, IDOperation);
            
            // Se a estação contiver a operação que o carro necessita
            if (FirstOpsAdress) {
                
                // como temos o adress da posiçao do caracter que corresponde a operação que queremos e o adress da primeira operação que a maquina faz podemos calcular a diferença
                // Index da posição da operação
                //     b - abc
                // ex: 22 -21 = 1
                int IndexOfFirstOpsAdress = (int) (FirstOpsAdress - (StationsArray[i].Ops));
                
                // Para saber o valor mais barato precisa de primeiro ter alguma coisa para comparar dai aqui serem atribuidos os valores da 1 estação que possa realizar a opereação mesmo sem ser a mais barata
                if (GetFistValue) {
                    StationIndex = i;
                    FirstOpsIndex = IndexOfFirstOpsAdress;
                    GetFistValue = 0;
                } else {
                    if  ((StationsArray[i].DurationOps[IndexOfFirstOpsAdress]*StationsArray[i].CostPerMin)<(StationsArray[i].DurationOps[IndexOfFirstOpsAdress]*StationsArray[StationIndex].CostPerMin)) {
                        StationIndex = i;
                        FirstOpsIndex = IndexOfFirstOpsAdress;
                    }
                }
                
            }
            
        }
        
    }

    // Os valores possivies sao -1 (Waiting) / 1,2,3,4,5 (stations)(Processing)
    IndexOfProcessingStationAndOps[0] = StationIndex;
    // Os valores possivies sao -1 (Waiting) / 0,1,2 (Time)(Processing)
    IndexOfProcessingStationAndOps[1] = FirstOpsIndex;
}

// Verifica se no tempo que o admin mandou passar existe algum carro no estado processing que acabe durate esse tempo
int TimeToOverTheQuickOperation(ProcessingStruct *P,int TimeToSkip) {
    
    int z,w;
    int TimeToQuickOps = -1;
    
    // adicionar aqui os valores que acabam primeiro que o TimeToSkip
    int *TimeToComplete = calloc(TotalOfProcessingStruct, sizeof(int));
    
    // Colocar os Valores no vetor
    for (z=0;z<TotalOfProcessingStruct;z++) {
        
        // So queremos os valores correspondentes ao Processing
        // Se a difernça for negativa ou seja acaba antes do tempo que o admin mandou passar
        if (((P+z)->TimeLeft-TimeToSkip<=0) && strlen((P+z)->State) == 10) {
            TimeToComplete[z] = (P+z)->TimeLeft;
        } else {
            TimeToComplete[z] = -1;
        }
    }
    
    // verificar qual dos que acaba antes do tempo do admin, acaba mais cedo
    for(w=0;w<TotalOfProcessingStruct;w++) {
        
        // quer dizer que nao acabou antes do tempo do admin
        if (TimeToComplete[w]!=-1) {
            // guardar o 1 valor
            if (TimeToQuickOps == -1) {
                TimeToQuickOps = TimeToComplete[w];
            } else if (TimeToComplete[w]<TimeToQuickOps) {
                TimeToQuickOps = TimeToComplete[w];
            }
        }
    }
    
    free(TimeToComplete);
    // if TimeToQuickOps == -1
    //  nenhuma operação chega a ser completa
    // else
    // a operação chega a ser completa
    return TimeToQuickOps;
}

// *** FUNCOES DO MENU *** //


// .1
void ShowStock(StockStruct *S) {
    
    
    if (ReadStockFile == 0) {
        ReadStockFileForStruct(S);
    }
    
    int i;
    float TotalCost=0.0;
    
    printf("\n\t\t****STOCK****\n\n");
    printf("\tID\tType\t\tPrice\tQt.\n\n");
    
    for(i=1;i<7;i++){
        TotalCost += Price[i]*(S->Quantity[i]);
        printf("\t%d\t%s\t%.2f\t%d\n",i,TypeNames[i],Price[i],S->Quantity[i]);
    }

    printf("\nTotal cost: %.2f€\n",TotalCost);
    
    // Mandar para o menu
    char Done;
    fseek(stdin,0,SEEK_END);
    scanf("%c",&Done);
    Menu();
}


// .2
void RefillStock(StockStruct *S) {
    
    char RefillFileName[20];
    
    printf("Filename: ");
    
    fseek(stdin,0,SEEK_END);
    gets(RefillFileName);
    
    if (strlen(RefillFileName) == 0) {
        Menu();
    }
    
    Filetxt = fopen(RefillFileName, "r");
    
    if (Filetxt == NULL) {
        printf("Não foi possivel encontrar/abrir esse ficheiro ERRO 5");
        exit(5);
    }
    
    QuantityStockForStruct(S);

    fclose(Filetxt);
    Menu();
}


// .3
void BuildCar(StockStruct *Stock, ProcessingStruct *Processing) {
    
    // verificar se existe material disponivel
    // se sim remove os materiais necessarios para construção
    // Chama o file processing se a estrutura estiver vazia e o StationFile
    // coloca o file processing numa struct e o binary na struct
    // Ver quais sao as estaçoes que estao disponiveis
    // Escolher a estação disponivel e mais barata com a 1 operação a realizar
    
    char AdminChoice;
    
    int ConvertCharToIntAdminChoice = 0;
    
    puts("\n\t\t***Product List Menu***\n");
    
    puts("\t1 - Renault");
    puts("\t2 - Mercedes");
    puts("\t3 - Honda");
    puts("\t4 - Ford");
    puts("\t5 - Ferrari");
    puts("\n\tB - Back\n");
    
    printf("Option:");
    
    scanf(" %c",&AdminChoice);
    
    // 49 -> 1 // 53 -> 5
    if ((AdminChoice>=49) &&(AdminChoice<=53)) {
        
        ConvertCharToIntAdminChoice = AdminChoice-48;
        
        if (ReadStockFile == 0) {
            ReadStockFileForStruct(Stock);
        }
        
        if (CheckStockIsAvailable(Stock,ConvertCharToIntAdminChoice)) {
            
            // Como tem em Stock os materias e ja foram removidos
            if (TotalOfProcessingStruct == 0) {
                ReadProcessingFileForStruct(Processing);
                ReadStationBinaryFileForStruct();
            }
            
            // Ja temos os dados todos entao podemos produzir
            
            // Vamos buscar mais memoria (so precisamos de +1 pois so adicionamos 1 carro de cada vez)
            // https://stackoverflow.com/questions/3827892/how-can-i-change-the-size-of-an-array-in-c
            // https://stackoverflow.com/questions/6169972/realloc-an-array-of-structs
            ProcessingStruct * ProcessingIncrease = realloc(Processing,((1)*sizeof(ProcessingStruct)));
            Processing = ProcessingIncrease;
            
            // Se nao colocarmos isto ele da erro
            if (Processing == NULL) {
                printf("Não foi possivel criar um array com memoria dinamica ERRO 6");
                exit(6);
            }
            
            // 1-> Verificar as estaçoes disponiveis
            StationsAvailable(Processing);
            
            // 2-> Das estações disponiveis procurar a que faz a operação pretendida
            StationWithOperationFreeAndCheap(CarsOperations[ConvertCharToIntAdminChoice][0]);
            
            // Adicionar mais 1 elemento
            ++TotalOfProcessingStruct;
            
            // temos de retirar 1 a todos pois o index do array começa no 0
            // Nome do carro
            strcpy((Processing+TotalOfProcessingStruct-1)->Type, CarsNames[ConvertCharToIntAdminChoice]);
            
            // Como estamos a criar o carro so pode ter 2 valores possiveis (Processing ou Waiting)
            
            // Waiting quando StationIndex for == -1 (WAITING) // Se nao ouver estação disponivel
            if (IndexOfProcessingStationAndOps[0] == -1) {
                strcpy((Processing+TotalOfProcessingStruct-1)->State, CarsStates[2]);
                (Processing+TotalOfProcessingStruct-1)->Local = '-';
                (Processing+TotalOfProcessingStruct-1)->TimeLeft = 0;
                
                // Processing quando StationIndex for != -1 (PROCESSING) // Se ouver estação disponivel
            } else {
                strcpy((Processing+TotalOfProcessingStruct-1)->State, CarsStates[1]);
                (Processing+TotalOfProcessingStruct-1)->Local = (IndexOfProcessingStationAndOps[0]+1);
                
                (Processing+TotalOfProcessingStruct-1)->TimeLeft = StationsArray[IndexOfProcessingStationAndOps[0]].DurationOps[IndexOfProcessingStationAndOps[1]];
            }
            
            // é igual para o (Processing ou Waiting)
            // Pois o index do CarsStates so tem valor apartir do 1
            strcpy((Processing+TotalOfProcessingStruct-1)->OperationsLeft, CarsOperations[ConvertCharToIntAdminChoice]);
            
            
            //->  APENAS SERVE PRA VER SE OS CARROS PASSAM PARA CONSTRUÇÃO
            int z;
            for(z=0; z<TotalOfProcessingStruct; z++) {
                if (Processing[z].Local != '-') {
                    printf("valores posição1 %d -> %s %s %c %d %s\n",z,Processing[z].Type,Processing[z].State,Processing[z].Local+48,Processing[z].TimeLeft,Processing[z].OperationsLeft);
                } else {
                    printf("valores posição1 %d -> %s %s %c %d %s\n",z,Processing[z].Type,Processing[z].State,Processing[z].Local,Processing[z].TimeLeft,Processing[z].OperationsLeft);
                }
            }
            
            printf("The product is now in production!\n");
            printf("Product is now in station 1\n");

            Menu();
            
        } else {
            Menu();
        }
        
    } else {
        Menu();
    }
}

// 4

// 4.1
void ShowAllProcesses() {
    
    puts("\n\t\t***PROCESSES***\n");
    
    puts("\nType\t\tState\t\tLocal\t Rem.Time\tOps.\n");
    
    int t;
    for(t=0; t<TotalOfProcessingStruct; t++) {
        
        if (Processing[t].Local != '-') {
            printf("%-10s %-14s %c\t\t\t%d\t\t%s\n",Processing[t].Type,Processing[t].State,Processing[t].Local+48,Processing[t].TimeLeft,Processing[t].OperationsLeft);
        } else {
            printf("%-10s %-14s %c\t\t\t%d\t\t%s\n",Processing[t].Type,Processing[t].State,Processing[t].Local,Processing[t].TimeLeft,Processing[t].OperationsLeft);
        }
        
    }
    
    char ToMenu[20];
    fseek(stdin,0,SEEK_END);
    gets(ToMenu);
    
    if (strlen(ToMenu) == 0) {
        FactoryState();
    }
}


// 4.2
void ShowProcessesByType(char Type[]) {
    
    puts("\n\t\t***PROCESSES***\n");
    
    puts("\nType\t\tState\t\tLocal\t Rem.Time\tOps.\n");

    int t;
    for(t=0; t<TotalOfProcessingStruct; t++) {
        
        if (strcmp(Processing[t].Type, Type) == 0) {
            if (Processing[t].Local != '-') {
                printf("%-10s %-14s %c\t\t\t%d\t\t%s\n",Processing[t].Type,Processing[t].State,Processing[t].Local+48,Processing[t].TimeLeft,Processing[t].OperationsLeft);
            } else {
                printf("%-10s %-14s %c\t\t\t%d\t\t%s\n",Processing[t].Type,Processing[t].State,Processing[t].Local,Processing[t].TimeLeft,Processing[t].OperationsLeft);
            }
        }
    }
    
    char ToMenu[20];
    fseek(stdin,0,SEEK_END);
    gets(ToMenu);
    
    if (strlen(ToMenu) == 0) {
        FactoryState();
    }
    
}


// 4.3
void ShowProcessesByState(char State[]) {
    
    puts("\n\t\t***PROCESSES***\n");
    
    puts("\nType\t\tState\t\tLocal\t Rem.Time\tOps.\n");

    int t;
    for(t=0; t<TotalOfProcessingStruct; t++) {
        
        if (strcmp(Processing[t].State, State) == 0) {
            if (Processing[t].Local != '-') {
                printf("%-10s %-14s %c\t\t\t%d\t\t%s\n",Processing[t].Type,Processing[t].State,Processing[t].Local+48,Processing[t].TimeLeft,Processing[t].OperationsLeft);
            } else {
                printf("%-10s %-14s %c\t\t\t%d\t\t%s\n",Processing[t].Type,Processing[t].State,Processing[t].Local,Processing[t].TimeLeft,Processing[t].OperationsLeft);
            }
        }
    }
    
    char ToMenu[20];
    fseek(stdin,0,SEEK_END);
    gets(ToMenu);
    
    if (strlen(ToMenu) == 0) {
        FactoryState();
    }
}


// 4.4
void MachineState() {
    
    puts("\n\t\t***FACTORY STATE***\n");
    
    puts("\nStation Nº\t\tState\t\t\tType\t\tOp.\n");
    
    char State[5][11];
    char Type[5][9];
    char Ops[5];
    
    // Inicializar as estaçoes todas como se estivessem vazias
    int i;
    for(i=0;i<5;i++) {
        strcpy(State[i], "Idle");
        strcpy(Type[i], "-");
        strcpy(&Ops[i], "-");
    }
    
    
    for(i=0; i<TotalOfProcessingStruct; i++) {
        
        // Quer dizer que tem estação
        if (Processing[i].Local != '-') {
            
            // colocar os valores na posição da estação
            strcpy(State[Processing[i].Local-1], Processing[i].State);
            strcpy(Type[Processing[i].Local-1], Processing[i].Type);
            
            // Saber qual a proxima operação a ser feita
            int x = 0;
            while ((Processing[i].OperationsLeft[x] == '-') && (x<4)) {
                x++;
            }
            
            Ops[Processing[i].Local-1] = Processing[i].OperationsLeft[x];
        }
        
    }
    
    for(i=0;i<5;i++) {
        printf("\t%-12d%-17s%-12s%c\n",i+1,State[i],Type[i],Ops[i]);
    }
    
    char ToMenu[20];
    fseek(stdin,0,SEEK_END);
    gets(ToMenu);
    
    if (strlen(ToMenu) == 0) {
        FactoryState();
    }
    
}


// 4.5
void MachineInfo() {
    
    puts("\n\t\t****STATIONS****\n");
    
    puts("\nID\t\tOperations\t\tDurations\tCost.Min\tOper.Time\n");
    
    int x,j;
    for(x=0;x<5;x++) {
        
        printf(" %-11d%-13s",StationsArray[x].ID,StationsArray[x].Ops);
        
        for(j=0;j<3;j++) {
            if(j<2){
                printf("%d-",StationsArray[x].DurationOps[j]);
            } else {
                printf("%-8d",StationsArray[x].DurationOps[j]);
            }
        }
        
        printf("%-13.2f%d\n",StationsArray[x].CostPerMin,StationsArray[x].TotalDurationOps);
    }
    
    char ToMenu[20];
    fseek(stdin,0,SEEK_END);
    gets(ToMenu);
    
    if (strlen(ToMenu) == 0) {
        FactoryState();
    }
    
}


// 4.6
void Map() {
    
    puts("\n\t***W A R E H O U S E***\n");
    
    puts("\n  1\t    2\t  3\t    4\t  5");
    
    char Ops[5];
    
    // Inicializar  como se estivessem vazias
    int i;
    for(i=0;i<5;i++) {
        strcpy(&Ops[i], "-");
    }
    
    for(i=0; i<TotalOfProcessingStruct; i++) {
        
        // Quer dizer que tem estação
        if (Processing[i].Local != '-') {
            
            // Saber qual a proxima operação a ser feita
            int x = 0;
            while ((Processing[i].OperationsLeft[x] == '-') && (x<4)) {
                x++;
            }
            
            Ops[Processing[i].Local-1] = Processing[i].OperationsLeft[x];
        }
        
    }
    
    int j;
    for(j=0;j<5;j++) {
        printf(" ---  ");
    }
    
    printf("\n");
    // colocar no sitio certo
    for(i=0;i<5;i++) {
        printf("| %c | ",Ops[i]);
    }
    
    printf("\n");
    for(j=0;j<5;j++) {
        printf(" ---  ");
    }
    
    // Voltar ao menu FactoryState
    char ToMenu[20];
    fseek(stdin,0,SEEK_END);
    gets(ToMenu);
    
    if (strlen(ToMenu) == 0) {
        FactoryState();
    }
    
}


// 5.

// 5.1
void FinishedProductByType() {
    
    puts("\n\t\t*****STATE CHART*****\n");
    
    // inicializar a 0 // O index 0 nao existe
    int CarsNumbers[6] = {};
    int TotalCars = 0;

    int i;
    for(i=0; i<TotalOfProcessingStruct; i++) {
        
        // Finished
        if (strlen(Processing[i].State) == 8) {
            
            if (strcmp(Processing[i].Type, CarsNames[1]) == 0) {
                // Renault
                CarsNumbers[1]++;
            } else if (strcmp(Processing[i].Type, CarsNames[2]) == 0) {
                // Mercedes
                CarsNumbers[2]++;
            } else if (strcmp(Processing[i].Type, CarsNames[3]) == 0) {
                // Honda
                CarsNumbers[3]++;
            } else if (strcmp(Processing[i].Type, CarsNames[4]) == 0) {
                // Ford
                CarsNumbers[4]++;
            } else if (strcmp(Processing[i].Type, CarsNames[5]) == 0) {
                // Ferrari
                CarsNumbers[5]++;
            }
        }
    }
    
    int l;
    for(l=1;l<6;l++) {
        TotalCars += CarsNumbers[l];
    }
    // TotalCars -> ***** (5)
    // valor -> (int)x
    
    // Se ja houverem que ja tenham terminado
    if (TotalCars!=0) {
      
        int g,q;
        for(g=1;g<6;g++) {
            printf("\n%-10s ",CarsNames[g]);
            for(q=0;q<(int)(CarsNumbers[g]*5/TotalCars);q++) {
                printf("*");
            }
        }

        puts("\n\nNum\t\tMarca\t\tQtd.\n");
        
        int d;
        for (d=1;d<6;d++) {
            printf(" %-6d %-13s%d\n",d,CarsNames[d],CarsNumbers[d]);
        }
     
    } else {
        printf("Nao existem carros que ja acabaram o fabrico\n");
    }
    
    char ToMenu[20];
    fseek(stdin,0,SEEK_END);
    gets(ToMenu);
    
    if (strlen(ToMenu) == 0) {
        Stats();
    }
    
}


// 5.2
void ProductsState() {
    
    puts("\n\t\t*****STATE CHART*****\n");
    
    int Count[3] = {};
    // 0 -> Finished
    // 1 -> Processing
    // 2 -> Waiting

    int i;
    
    for(i=0; i<TotalOfProcessingStruct; i++) {
        
        if (strlen(Processing[i].State) == 10) {
            // Processing
            Count[1]++;
            
        } else if (strlen(Processing[i].State) == 7) {
            // Waiting
            Count[2]++;
            
        } else {
            // Finished
            Count[0]++;
        }
    }
    
    
    if (TotalOfProcessingStruct !=0) {
        
        // Total -> ***** (5)
        // valor -> (int)x
        
        int g,q;
        for(g=0;g<3;g++) {
            printf("\n%-12s ",CarsStates[g]);
            for(q=0;q<(int)(Count[g]*5/TotalOfProcessingStruct);q++) {
                printf("*");
            }
        }
        
    } else {
        printf("Nao existem carros na estrutura \n");
    }
    
    char ToMenu[20];
    fseek(stdin,0,SEEK_END);
    gets(ToMenu);
    
    if (strlen(ToMenu) == 0) {
        Stats();
    }
    
}


// 5.3
void StationsCost() {
    
    puts("\n\t\t****Stations Info****\n");
    
    puts("\nStation ID\t\tOp.Time\t\t\tOp.Cost\n");
    
    int i,CountAux,CountTotalHours = 0;
    int CountTotalPrice = 0;
    
    for(i=0;i<5;i++) {
        
        CountAux = StationsArray[i].TotalDurationOps*StationsArray[i].CostPerMin;
        CountTotalPrice += CountAux;
        CountTotalHours += StationsArray[i].TotalDurationOps;
        printf(" %-17d%-15d%d€\n",i+1,StationsArray[i].TotalDurationOps,CountAux);
    }
    
    printf("\nTotal operating time: %d mins\n",CountTotalHours);
    printf("Total cost:\t\t\t  %d€\n",CountTotalPrice);
    
    
    char ToMenu[20];
    fseek(stdin,0,SEEK_END);
    gets(ToMenu);
    
    if (strlen(ToMenu) == 0) {
        Stats();
    }
}


// .6
void SkipTime(ProcessingStruct *P) {
    
    // verificar se existe algum carro que termine a operação antes do tempo que o utilizador mandou passar
    // Se sim retira o tempo necessario para o carro com o menor tempo terminar ( depois verifica se essaa foi a ultima operação)
        // se sim passa para o estado waiting
        // se nao verifica se existe alguma estaçao disponivel (escolhe a mais barata) para efetuar a proxima operaçao
            // se sim passa a processing
            // se nao passa a waiting
        // No final calcula o tempo que ainda falta passar com a diferença entre o valor inicial e aquele que usamos para verficar o carro que termina primeiro
        // Faz isso ate ja nao haver nenhum carro que termine a operação depois so retira o tempo que falta a todos os carros em processing
    // Se nao basta apenas descontar o tempo que o admin descontou a todos os carros em processing
    
    if (TotalOfProcessingStruct == 0) {
        ReadProcessingFileForStruct(P);
        ReadStationBinaryFileForStruct();
    }
    
    int TimeToSkip;
    int TimeToSkipLater = -1;

    printf("How much time to skip(mins): ");
    scanf(" %d",&TimeToSkip);

    // Se estiver -1 é porque nao existe mais nenhuma mudança de operação
    // Aqui temos o tempo necessario para um dos carros terminar a operação
    TimeToSkipLater = TimeToOverTheQuickOperation(P,TimeToSkip);

    while (TimeToSkipLater != -1) {
        
        int g;
        for(g=0;g<TotalOfProcessingStruct;g++) {
            
            // So os Processing é que descontam o tempo
            if (strlen((P+g)->State) == 10) {
                (P+g)->TimeLeft = (P+g)->TimeLeft - TimeToSkipLater;
                
                if (((P+g)->Local) != '-' ) {
                    // Total tempo que a estação esteve a trabalhar
                    StationsArray[(P+g)->Local-1].TotalDurationOps = StationsArray[(P+g)->Local-1].TotalDurationOps + TimeToSkipLater;
                }
                
                // Bloquear a operação que acabamos de fazer
                int x = 0;
                while (((P+g)->OperationsLeft[x] == '-') && (x<4)) {
                    x++;
                }
                
                // Quer dizer que estes carros chegaram ao final desta operação e ao final das operaçoes (Finished)
                if (((P+g)->TimeLeft==0) && (x==2)) {
                    strcpy((P+g)->State, CarsStates[0]);
                    (P+g)->Local = '-';
                    (P+g)->TimeLeft = 0;
                    (P+g)->OperationsLeft[x] = '-';
                    // Quer dizer que estes carros chegaram ao final desta operação vamos passar-los para o estado (Waiting)
                } else if (((P+g)->TimeLeft==0) && (x<2)){
                    strcpy((P+g)->State, CarsStates[2]);
                    (P+g)->Local = '-';
                    (P+g)->TimeLeft = 0;
                    (P+g)->OperationsLeft[x] = '-';
                }
            }
        }
        
        // Agora os carros que passaram do Processing para o estado Waiting desocuparam estaçoes dai ser possivel que alguns carros no estado waiting possam passar para o processing
        int h;
        for(h=0;h<TotalOfProcessingStruct;h++) {
            
            // So os Waiting é que fazem
            if (strlen((P+h)->State) == 7) {

                // 1-> Verificar as estaçoes disponivieis
                StationsAvailable(P);
                
                // Saber qual a proxima operação a ser feita
                int x = 0;
                while (((P+h)->OperationsLeft[x] == '-') && (x<4)) {
                    x++;
                }

                // 2-> Das estações disponiveis procurar a que faz a operação pretendida
                StationWithOperationFreeAndCheap((P+h)->OperationsLeft[x]);
                
                // Processing quando StationIndex for != -1 (PROCESSING) // Se ouver estação disponivel
                if (IndexOfProcessingStationAndOps[0] != -1) {
                    strcpy((P+h)->State, CarsStates[1]);
                    (P+h)->Local = (IndexOfProcessingStationAndOps[0]+1);
                    (P+h)->TimeLeft = StationsArray[IndexOfProcessingStationAndOps[0]].DurationOps[IndexOfProcessingStationAndOps[1]];
                }
                // Se nao houver estação disponivel nao faz nada fica na mesma
            }

        }
        
        //EX: 25 = 30 - 5
        // A tribuir o tempo restante
        TimeToSkip = TimeToSkip-TimeToSkipLater;
        
        // Aqui vai ver se ainda tem mudança de operaçoes caso nao tenha guarda o tempo que ainda temos de retirar dos outros processing
        if(TimeToOverTheQuickOperation(P,TimeToSkip) == -1) {
            // Passa para a remoção de tempo sem alteração das operaçoes no (Processing)
            break;
        } else {
            TimeToSkipLater = TimeToOverTheQuickOperation(P,TimeToSkip);
        }
    }

    // quer dizer que ainda temos de subtrair o tempo que o admin mandou avançar mas sem mudar de operação só nos (Processing)
    if (TimeToSkip) {
        int r;
        for(r=0;r<TotalOfProcessingStruct;r++) {
            if (strlen((P+r)->State) == 10) {
                (P+r)->TimeLeft = (P+r)->TimeLeft - TimeToSkip;
                
                if (((P+r)->Local) != '-' ) {
                    // Total tempo que a estação esteve a trabalhar
                    StationsArray[(P+r)->Local-1].TotalDurationOps = StationsArray[(P+r)->Local-1].TotalDurationOps + TimeToSkip;
                }
                
            }
        }
    }


    // APENAS SERVE PARA VER SE O TIMESKIP FUNCIONA CORRETAMENTE
    int t;
    for(t=0; t<TotalOfProcessingStruct; t++) {
        
        if (Processing[t].Local != '-') {
            printf("valores posição2 %d -> %s %s %c %d %s\n",t,Processing[t].Type,Processing[t].State,Processing[t].Local+48,Processing[t].TimeLeft,Processing[t].OperationsLeft);
        } else {
            printf("valores posição2 %d -> %s %s %c %d %s\n",t,Processing[t].Type,Processing[t].State,Processing[t].Local,Processing[t].TimeLeft,Processing[t].OperationsLeft);
        }
    }
    
    char ToMenu[20];
    fseek(stdin,0,SEEK_END);
    gets(ToMenu);
    
    if (strlen(ToMenu) == 0) {
        Menu();
    }
    
}


// .E
void SaveStationsFileWhenAdminOut() {
    
    Filetxt = fopen("Stations.dat", "wb");
    
    // Rescrever a estrutura no file Stock
    if (Filetxt == NULL) {
        printf("Não foi possivel criar um array com memoria dinamica ERRO 9");
        exit(9);
    }
    
    if (fwrite(StationsArray, sizeof(StationStruct), 5, Filetxt) != 5) {
        printf("nao foram escritos todas as estaçoes ERRO 6");
        exit(6);
    }
    

    fflush(Filetxt);
    fclose(Filetxt);
}

// .E
void SaveStockFileWhenAdminOut(StockStruct *S) {
    
    Filetxt = fopen("Stock.txt", "w");
    
    // Rescrever a estrutura no file Stock
    if (Filetxt == NULL) {
        printf("Não foi possivel criar um array com memoria dinamica ERRO 7");
        exit(7);
    }
    
    int i;
    
    // Vai começar no ID 1
    for(i=1;i<7;i++){
        
        int * QuantitySub = S->Quantity+i;
        
        // Vai remover 1 a 1 da estrutura e adicionar
        // Enquanto o Valor armazenado no Id for diferente de 0 ele vai imprimir no file
        while (*QuantitySub) {
            
            fprintf(Filetxt, "\n%d %s %.2f",i,TypeNames[i],Price[i]);
            --(*QuantitySub);
        }
        
    }
    
    fflush(Filetxt);
    fclose(Filetxt);
}

// .E
void SaveProcessingFileWhenAdminOut(ProcessingStruct *P) {
    
    Filetxt = fopen("Processing.txt", "w");
    
    // Rescrever a estrutura no file Processing
    if (Filetxt == NULL) {
        printf("Não foi possivel criar um array com memoria dinamica ERRO 8");
        exit(8);
    }
    
    int i;
    
    int LoopElements = TotalOfProcessingStruct;
    
    for(i=0;i<(LoopElements);i++){
        
        // Aqui so faz na ultima posiçao para nao deixar espaço no processing pode dar erro na proxima vez pois vai contar + 1 linha em branco
        if (i == LoopElements-1) {
            
            if((P+i)->Local != '-') {
                
                // APENAS SERVE PARA TESTAR SE OS VALORES ESTAO CORRETOS
                printf("valores posição3 -> %s %s %c %d %s",(P+i)->Type,(P+i)->State,(P+i)->Local+48,(P+i)->TimeLeft,(P+i)->OperationsLeft);

                
                fprintf(Filetxt, "%s %s %c %d %s",(P+i)->Type,(P+i)->State,(P+i)->Local+48,(P+i)->TimeLeft,(P+i)->OperationsLeft);
            } else {
                
                // APENAS SERVE PARA TESTAR SE OS VALORES ESTAO CORRETOS
                printf("valores posição3 -> %s %s %c %d %s",(P+i)->Type,(P+i)->State,(P+i)->Local,(P+i)->TimeLeft,(P+i)->OperationsLeft);
                
                fprintf(Filetxt, "%s %s %c %d %s",(P+i)->Type,(P+i)->State,(P+i)->Local,(P+i)->TimeLeft,(P+i)->OperationsLeft);
            }
            
        } else {
            
            if((P+i)->Local != '-') {
                
                printf("valores posição3 -> %s %s %c %d %s\n",(P+i)->Type,(P+i)->State,(P+i)->Local+48,(P+i)->TimeLeft,(P+i)->OperationsLeft);

                
                fprintf(Filetxt, "%s %s %c %d %s\n",(P+i)->Type,(P+i)->State,(P+i)->Local+48,(P+i)->TimeLeft,(P+i)->OperationsLeft);
            } else {
                
                printf("valores posição3 -> %s %s %c %d %s\n",(P+i)->Type,(P+i)->State,(P+i)->Local,(P+i)->TimeLeft,(P+i)->OperationsLeft);

                
                fprintf(Filetxt, "%s %s %c %d %s\n",(P+i)->Type,(P+i)->State,(P+i)->Local,(P+i)->TimeLeft,(P+i)->OperationsLeft);
            }
        }
    }
    
    fflush(Filetxt);
    fclose(Filetxt);
}


int main() {
    
    // Ver o Tamanho do vetor de estruturas que vai conter os carros
    int Lenght = ProcessingFileLengthOfArrayStruct();
    
    //** Definir o vetor de arrays
    
    // Como vai ser possivel adicionar ao Stock novos carros teremos de usar memoria dinamica para podermos alterar o tamanho do array mais adiante
    // https://stackoverflow.com/questions/3827892/how-can-i-change-the-size-of-an-array-in-c
    // Temos de multiplicar o numero de linhas (elementos) com numero de bytes da estrutura
    //https://stackoverflow.com/questions/21001568/uninitialised-array-using-malloc
    // O malloc inicializa o valor com uns random ja o calloc inicializa a 0
    ProcessingStruct* ProcessingCalloc =calloc(Lenght, sizeof(ProcessingStruct));
    Processing = ProcessingCalloc;
    
    // Se nao colocarmos isto ele da erro
    if (Processing == NULL) {
        printf("Não foi possivel criar um array com memoria dinamica ERRO 9");
        exit(9);
    }

    Menu();
    
    return 0;
}

// .4
void FactoryState() {
    
    char AdminChoice;
    
    puts("\n\t\t***Factory State Menu***\n");
    
    puts("\t1 - Show all processes");
    puts("\t2 - Show processes by type");
    puts("\t3 - Show processes by state");
    puts("\t4 - Machine state");
    puts("\t5 - Machine info");
    puts("\t6 - Map");

    puts("\n\tB - Back\n");
    
    printf("Option:");
    
    scanf(" %c",&AdminChoice);
    
    
    // Como tem em Stock os materias e ja foram removidos
    if (TotalOfProcessingStruct == 0) {
        ReadProcessingFileForStruct(Processing);
        ReadStationBinaryFileForStruct();
    }
    
    // 49 -> 1 // 54 -> 6
    while ( ((AdminChoice != 'b') && (AdminChoice != 'E') && !(AdminChoice >= 49 && AdminChoice<=54) ) ) {
        
        printf("Invalid Option %c \n",AdminChoice);
        
        // Enquanto nao
        printf("Option: ");
        scanf(" %c",&AdminChoice);
    }
    
    char Type[9];
    char State[11];

    switch (AdminChoice) {
        case '1':
            ShowAllProcesses();
            break;
        case '2':
            printf("Type: ");
            scanf(" %s",Type);
            ShowProcessesByType(Type);
            break;
        case '3':
            printf("State: ");
            scanf(" %s",State);
            ShowProcessesByState(State);
            break;
        case '4':
            MachineState();
            break;
        case '5':
            MachineInfo();
            break;
        case '6':
            Map();
            break;
        case 'b':
            Menu();
            break;
        case 'B':
            Menu();
            break;
    }
}


// .5
void Stats() {
    
    char AdminChoice;
    
    puts("\n\t\t***Factory State Menu***\n");
    
    puts("\t1 - Finished products by type");
    puts("\t2 - Products state");
    puts("\t3 - Stations cost");
    
    puts("\n\tB - Back\n");
    
    printf("Option:");
    
    scanf(" %c",&AdminChoice);
    
    
    // Como tem em Stock os materias e ja foram removidos
    if (TotalOfProcessingStruct == 0) {
        ReadProcessingFileForStruct(Processing);
        ReadStationBinaryFileForStruct();
    }
    
    // 49 -> 1 // 54 -> 6
    while ( ((AdminChoice != 'b') && (AdminChoice != 'E') && !(AdminChoice >= 49 && AdminChoice<=54)) ) {
        
        printf("Invalid Option %c \n",AdminChoice);
        
        // Enquanto nao
        printf("Option: ");
        scanf(" %c",&AdminChoice);
    }
    
    switch (AdminChoice) {
        case '1':
            FinishedProductByType();
            break;
        case '2':
            ProductsState();
            break;
        case '3':
            StationsCost();
            break;
        case 'b':
            Menu();
            break;
        case 'B':
            Menu();
            break;
    }
}

// Se ele nao quizer aqui podemos criar um while no main e faz break quando clicamos no e ou E
void Menu() {
    
    char AdminChoice;
    
    // Menu
    puts("\n\t\t***Factory Admin Menu***\n");
    
    puts("\t1 - Show stock");
    puts("\t2 - Refill stock");
    puts("\t3 - Build car");
    puts("\t4 - Factory State");
    puts("\t5 - Stats");
    puts("\t6 - Time skip\n");
    
    puts("\tE - Exit\n");
    
    printf("Option:");
    
    scanf(" %c",&AdminChoice);
    
    
    // 1 -> 49   // 6 -> 54
    while ( ((AdminChoice != 'e') && (AdminChoice != 'E') && !(AdminChoice >= 49 && AdminChoice<=54) ) ) {
        
        printf("Invalid Option %c \n",AdminChoice);
        
        // Enquanto nao
        printf("Option: ");
        scanf(" %c",&AdminChoice);
    }
    
    switch (AdminChoice) {
        case '1':
            ShowStock(&Stock);
            break;
        case '2':
            RefillStock(&Stock);
            break;
        case '3':
            BuildCar(&Stock,Processing);
            break;
        case '4':
            FactoryState();
            break;
        case '5':
            Stats();
            break;
        case '6':
            SkipTime(Processing);
            break;
        case 'e':
            // so faz se tivermos criado carros ou lido o ficheiro processing e passado para a estrutura
            if (TotalOfProcessingStruct != 0) {
                // Nao sei se devo usar
                // free(Processing);
                SaveStockFileWhenAdminOut(&Stock);
                SaveProcessingFileWhenAdminOut(Processing);
                SaveStationsFileWhenAdminOut();
            }
                printf("\nexiting...");
            break;
        case 'E':
            // so faz se tivermos criado carros ou lido o ficheiro processing e passado para a estrutura
            if (TotalOfProcessingStruct != 0) {
                // Nao sei se devo usar
                // free(Processing);
                SaveStockFileWhenAdminOut(&Stock);
                SaveProcessingFileWhenAdminOut(Processing);
                SaveStationsFileWhenAdminOut();
            }
                printf("\nexiting...");
            break;
    }
}

