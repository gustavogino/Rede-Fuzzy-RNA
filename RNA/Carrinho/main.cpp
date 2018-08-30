#include <iostream>
#include <vector>
#include <cmath>
#include <conio.h>


#include "neural_net.h"

int menu();

double sigmoid(double x)
{
  return 1 / (1 + exp(-x));
}

double d_dx_Sigmoid(double x)
{
    return sigmoid(x)*(1-sigmoid(x));
}

int main()
{
    double input[] =
    {
        0.0f/300.0f,
        50.0f/300.0f,
        100.0f/300.0f,
        150.0f/300.0f,
        200.0f/300.0f,
        250.0f/300.0f,
        300.0f/300.0f
    };
    std::vector< std::vector<double> > inputData( 7 );
    for (unsigned int i=0; i<inputData.size(); i++)
    {
        inputData[i].resize(1);
        for (unsigned int j=0; j<inputData[i].size(); j++)
            inputData[i][j] = input[ (inputData.front().size())*i + j ];
    }

    double output[] =
    {
        ( -255 +255)/510.0f , ( -255 +255)/510.0f,
        ( 128 +255)/510.0f , ( 64 +255)/510.0f,
        ( 128 +255)/510.0f , ( 64 +255)/510.0f,
        ( 64 +255)/510.0f , ( 128 +255)/510.0f,
        ( 64 +255)/510.0f , ( 128 +255)/510.0f,
        ( 200 +255)/510.0f , ( 200 +255)/510.0f,
        ( 255 +255)/510.0f , ( 255 +255)/510.0f,
    };
    std::vector< std::vector<double> > outputData( 7 );
    for (unsigned int i=0; i<outputData.size(); i++)
    {
        outputData[i].resize(2);
        for (unsigned int j=0; j<outputData.front().size(); j++)
            outputData[i][j] = output[ (outputData.front().size())*i + j ];
    }

    /**Print input e output
    for (unsigned int i=0; i<inputData.size(); i++)
    {
        std::cout<<"|";
        for (unsigned int j=0; j<inputData[i].size(); j++)
            std::cout<<" "<<inputData[i][j];
        std::cout<<" |"<<std::endl;
    }
    std::cout<<std::endl;
    for (unsigned int i=0; i<outputData.size(); i++)
    {
        std::cout<<"|";
        for (unsigned int j=0; j<outputData[i].size(); j++)
            std::cout<<" "<<outputData[i][j];
        std::cout<<" |"<<std::endl;
    }
    ///--------------------*/

    unsigned int v[] = {1,5,2};
    std::vector<unsigned int> layerSize ( v , v + sizeof(v)/sizeof(unsigned int) );
    NeuralNet RNA( layerSize );
    RNA.setActivationFunction(sigmoid);
    RNA.setDerivativeActivationFunction(d_dx_Sigmoid);

    RNA.setEvaluationData(inputData, outputData);
    int opc = 0;
    while (opc!=8)
    {
        opc = menu();
        switch(opc)
        {
            case 1:
            {
                /**Stochastic Gradient Descent
                double d;
                unsigned int e;
                std::cout<<"Digite o valor da taxa de aprendizado: ";
                std::cin>>d;
                RNA.setLearningRate(d);
                std::cout<<"Digite o numero de epocas: ";
                std::cin>>e;
                RNA.setEpochs(e);
                std::cout<<"Digite o tamanho do minibatch: ";
                std::cin>>e;
                RNA.showProgress(true);
                RNA.evaluateNet(false);
                std::cout<<"\nTreinando..."<<std::endl;
                time_t now, then;
                now = time(NULL);
                RNA.trainSGD(inputData, outputData, e);
                then = time(NULL);
                std::cout<<'\a';
                std::cout<<std::endl<<"Rede treinada, tempo de treinamento: "<<then-now<<" segundos"<<std::endl;
                getch();
                ///-----------------------*/

                /// Gradient Descent
                double d;
                unsigned int e;
                std::cout<<"Digite o valor da taxa de aprendizado: ";
                std::cin>>d;
                RNA.setLearningRate(d);
                std::cout<<"Digite o numero de epocas: ";
                std::cin>>e;
                RNA.setEpochs(e);
                RNA.showProgress(true);
                std::cout<<"\nTreinando..."<<std::endl;
                time_t now, then;
                now = time(NULL);
                RNA.trainGD(inputData, outputData);
                then = time(NULL);
                std::cout<<'\a';
                std::cout<<std::endl<<"Rede treinada, tempo de treinamento: "<<then-now<<" segundos"<<std::endl;
                getch();
                ///-------------------------*/

                break;
            }
            case 2:
            {
                double d;
                std::vector<double> inputTest;
                std::vector<double> outputTest;

                std::cout<<"input = ";
                std::cin>>d;
                inputTest.push_back( d );
                RNA.use(inputTest, &outputTest);
                for (unsigned int i = 0; i<outputTest.size(); i++)
                    std::cout<<"output["<<i<<"] = "<<outputTest[i]*510-255<<std::endl;
                getch();
                break;
            }
            case 3:
            {
                RNA.evaluate();
                getch();
                break;
            }

            case 4:
            {
                RNA.showWeightsBias();
                getch();
                break;
            }
            case 5:
            {
                std::string name;
                std::cout<<"Exportar como: ";
                std::cin>>name;
                RNA.save(name);
                break;
            }
            case 6:
            {
                std::string name;
                std::cout<<"Importar qual arquivo: ";
                std::cin>>name;
                RNA.load(name);
                RNA.setActivationFunction(sigmoid);
                RNA.setDerivativeActivationFunction(d_dx_Sigmoid);
                break;
            }
            case 7:
            {
                RNA.reconstruct();
                break;
            }
            case 0:
            {
                RNA.showAll();
                getch();
                break;
            }
            default:
                break;
        }
    }
    return 0;
}

int menu()
{
    system("cls");
    std::cout<<"1 - Treinar RNA"<<std::endl;
    std::cout<<"2 - Usar RNA"<<std::endl;
    std::cout<<"3 - Avaliar desempenho"<<std::endl;
    std::cout<<"4 - Mostrar pesos e bias"<<std::endl;
    std::cout<<"5 - Exportar rede neural"<<std::endl;
    std::cout<<"6 - Importar rede neural"<<std::endl;
    std::cout<<"7 - Resetar rede neural"<<std::endl;
    std::cout<<"8 - Sair"<<std::endl;
    std::cout<<"\n>>";
    int opc;
    std::cin>>opc;
    return opc;
}

