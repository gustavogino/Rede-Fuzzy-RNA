#ifndef NEURAL_NET_H_INCLUDED
#define NEURAL_NET_H_INCLUDED

#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>

///Neural Net Activation Function
typedef double(*NNactivFunc) (double);

double cost(std::vector<double> desiredOut, std::vector<double> output)
{
    double sum = 0.0f;
    for (unsigned int i=0; i<desiredOut.size(); i++)
        sum += (desiredOut[i] - output[i]) * (desiredOut[i] - output[i]);
    return sum;
}

///Derivative of cost function C =  (1/n) * sum_{i=1}^{n} |y-a|^2
double cost_derivative (double desiredOut, double output)
{ return output - desiredOut; }

typedef struct
{
    std::vector< std::vector<double> > weight;
    std::vector<double> bias;
    std::vector<double> net;
    std::vector<double> activation;
}net_layer;

typedef struct
{
    std::vector< std::vector<double> > dw;
    std::vector<double> db;
    std::vector<double> delta;
}grad;


class NeuralNet
{
    public:
        NeuralNet(std::vector<unsigned int> layersSize);
        void construct(std::vector<unsigned int> layersSize);
        void reconstruct();

        void save(std::string NeuralNetName);
        void load(std::string NeuralNetName);

        void setActivationFunction(NNactivFunc activFunc) { activationFunction = activFunc; }
        void setDerivativeActivationFunction(NNactivFunc derivActivFunc) { derivativeActivationFunction = derivActivFunc; }
        void setEpochs (unsigned int n_epochs)  { epochs = n_epochs; }
        void setLearningRate (double learningRate) {learnRate = learningRate; }
        void showProgress(bool condition) { show_progress = condition; }
        void evaluateNet(bool condition);
        void setEvaluationData(std::vector< std::vector<double> > evaluationInput, std::vector< std::vector<double> > evaluationOutput);

        void trainGD(std::vector< std::vector<double> >inputData, std::vector< std::vector<double> >outputData);
        void trainSGD(std::vector< std::vector<double> >inputData, std::vector< std::vector<double> >outputData, unsigned int miniBatchSize);
        void use(std::vector<double> input, std::vector<double> *output);
        void evaluate();
        void showWeightsBias();
        void showAll();
    private:
        ///Functions
        void generateWeightsBias();
        void calculateOutput(std::vector<double> input);
        void backpropagation(std::vector<double> inputData,  std::vector<double> outputData);
        ///Variables
            ///Structure of network
            unsigned int nLayers;
            std::vector<unsigned int> layerSize;
            std::vector<net_layer> rede;
            std::vector<grad> gradiente;
            ///Evaluation data
            std::vector< std::vector<double> > evaluation_input;
            std::vector< std::vector<double> > evaluation_output;
            ///Functions
            NNactivFunc activationFunction;
            NNactivFunc derivativeActivationFunction;
            ///Hyper parameters
            double learnRate;
            unsigned int epochs;
            ///Conditions
            bool isTrained;
            bool isConstructed;
            bool show_progress;
            bool evaluate_net;
};

///Public Functions
NeuralNet::NeuralNet(std::vector<unsigned int> layers_sizes) ///YES
{
    construct(layers_sizes);
    show_progress = false;
    evaluate_net = false;
}

void NeuralNet::construct(std::vector<unsigned int> layers_sizes) ///YES
{
    unsigned int i;
    nLayers = layers_sizes.size();
    ///Security Checks---
    if (nLayers<2)
    {
        isConstructed = false;
        nLayers = 0;
        return;
    }
    for (i=0; i<nLayers; i++)
    {
        if (layers_sizes.at(i) == 0)
        {
            isConstructed = false;
            nLayers = 0;
            return;
        }
    }
    ///--------------------

    /*layerSize.clear();
    for (i=0; i<nLayers; i++)
        layerSize.push_back( layers_sizes.at(i) );*/
    layerSize = layers_sizes;

    unsigned int redeSize = nLayers - 1; //Removing the layer corresponding to the input
    rede.clear();
    rede.resize(redeSize);
    for (unsigned int layer=0; layer<nLayers-1; layer++)
    {
        rede[layer].net.resize(layerSize[layer+1],0);
        rede[layer].activation.resize(layerSize[layer+1],0);
    }

    learnRate = 0.0f;
    epochs = 0;
    isConstructed = true;
    isTrained = false;
    generateWeightsBias();
}

void NeuralNet::reconstruct()
{
    construct(layerSize);
}

void NeuralNet::save(std::string NeuralNetName) ///YES
{
    unsigned int layer, neuron, i;
    if (!isConstructed)
        return;
    std::string filename = "Neural Nets\\";
    filename.append(NeuralNetName);
    filename.append(".txt");
    std::ofstream out (filename.c_str(), std::ios::out | std::ios::trunc);
    if (!out.is_open())
    {
        return;
    }
    out<<'N';
    out<<'N';
    out<<'\n';
    out<<isTrained<<'\n';
    out<<nLayers<<'\n';
    for (layer=0; layer<nLayers; layer++)
        out<<layerSize[layer]<<' ';
    out<<'\n'<<'\n'<<'B';
    for (layer=0;layer<nLayers-1;layer++)
    {
        out<<'\n';
        for (neuron=0; neuron<layerSize[layer+1]; neuron++)
            out<<rede.at(layer).bias[neuron]<<' ';
    }
    out<<'\n'<<'\n'<<'\n'<<'W';
    for (layer=0;layer<nLayers-1;layer++)
    {
        out<<'\n';
        for (neuron=0; neuron<layerSize[layer+1]; neuron++)
        {
            for (i=0; i<layerSize[layer]; i++)
                out<<rede.at(layer).weight[neuron][i]<<' ';
            out<<'\n';
        }
    }

    out.close();
}

void NeuralNet::load(std::string NeuralNetName) ///YES
{
    std::string filename = "Neural Nets\\";
    filename.append(NeuralNetName);
    filename.append(".txt");
    std::ifstream in(filename.c_str());
    if (!in.is_open())
        return;

    char c=in.get();
    if (c!='N' || c!=in.get())
    {
        in.close();
        return;
    }

    isConstructed = false;
    isTrained = false;
    epochs = 0;
    learnRate = 0.0f;
    nLayers = 0;
    activationFunction = NULL;
    derivativeActivationFunction = NULL;
    layerSize.clear();
    rede.clear();

    std::streamsize infinite = std::numeric_limits<std::streamsize>::max();
    std::vector<double> tempVec;

    unsigned int layer, neuron, i;
    in>>isTrained;
    in>>nLayers;
    layerSize.resize(nLayers);
    for (layer = 0; layer<nLayers; layer++)
        in>>layerSize[layer];

    in.ignore(infinite, 'B');
    rede.resize(nLayers-1);
    for (layer=0; layer<nLayers-1; layer++)
    {
        tempVec.resize(layerSize[layer+1]);
        for (neuron=0; neuron<layerSize[layer+1]; neuron++)
            in>>tempVec[neuron];
        rede.at(layer).bias = tempVec;
    }

    in.ignore(infinite, 'W');
    for (layer=0; layer<nLayers-1; layer++)
    {
        std::vector< std::vector<double> > tempMat( layerSize[layer+1], std::vector<double>(layerSize[layer]) );
        for (neuron=0; neuron<layerSize[layer+1]; neuron++)
        {
            for (i=0; i<layerSize[layer]; i++)
                in>>tempMat[neuron][i];
        }
        rede.at(layer).weight = tempMat;
    }

    for (unsigned int layer=0; layer<nLayers-1; layer++)
    {
        rede[layer].net.resize(layerSize[layer+1],0);
        rede[layer].activation.resize(layerSize[layer+1],0);
    }

    isConstructed = true;
    in.close();
}

void NeuralNet::evaluateNet(bool condition)
{
    if (evaluation_input.empty() || evaluation_output.empty())
        return;
    evaluate_net = condition;
}

void NeuralNet::setEvaluationData(std::vector< std::vector<double> > evaluationInput, std::vector< std::vector<double> > evaluationOutput)
{
    if (evaluationInput.size() != evaluationOutput.size())
        return;
    if (evaluationInput.front().size() != layerSize.front() || evaluationOutput.front().size() != layerSize.back())
        return;
    evaluation_input = evaluationInput;
    evaluation_output = evaluationOutput;
}

void NeuralNet::trainGD(std::vector< std::vector<double> >inputData, std::vector< std::vector<double> >outputData)
{
    unsigned int layer, neuron, input, i;
    if (!isConstructed)
    {
        return;
    }
    if (learnRate == 0 || epochs == 0)
    {
        return;
    }
    unsigned int inputSize = inputData.size();
    if (inputSize != outputData.size())
        return;

    gradiente.resize(nLayers-1);
    for (layer=0; layer<nLayers-1; layer++)
    {
        gradiente[layer].dw = std::vector< std::vector<double> >( layerSize[layer+1], std::vector<double>(layerSize[layer]) );
        gradiente[layer].db = std::vector<double>( layerSize[layer+1] );
        gradiente[layer].delta = std::vector<double>( layerSize[layer+1] );
    }

    unsigned int cur_epoch = 0;
    while (cur_epoch++ < epochs)
    {
        for (layer = 0; layer<nLayers-1; layer++)
        {
            for (neuron = 0; neuron<layerSize[layer+1]; neuron++)
            {
                gradiente[layer].db[neuron] = 0;
                for (i = 0; i<layerSize[layer]; i++)
                    gradiente[layer].dw[neuron][i] = 0;
            }
        }
        for ( input = 0; input<inputSize; input++)
        {
            backpropagation(inputData[input], outputData[input]);
            for (layer = 0; layer<nLayers-1; layer++)
            {
                for (neuron = 0; neuron<layerSize[layer+1]; neuron++)
                {
                    if (layer==0)
                    {
                        for (i = 0; i<layerSize[layer]; i++)
                        gradiente[layer].dw[neuron][i] += inputData[input][i] * gradiente[layer].delta[neuron];
                    }
                    else
                    {
                        for (i = 0; i<layerSize[layer]; i++)
                            gradiente[layer].dw[neuron][i] += rede[layer-1].activation[i] * gradiente[layer].delta[neuron];
                    }
                    gradiente[layer].db[neuron] += gradiente[layer].delta[neuron];
                }
            }
        }

        for (layer = 0; layer<nLayers-1; layer++)
        {
            for (neuron = 0; neuron<layerSize[layer+1]; neuron++)
            {
                rede[layer].bias[neuron] -= (learnRate/inputSize) * gradiente[layer].db[neuron];
                for (i = 0; i<layerSize[layer]; i++)
                    rede[layer].weight[neuron][i] -= (learnRate/inputSize) * gradiente[layer].dw[neuron][i];
            }
        }
        if (show_progress)
            std::cout<<"\r"<<cur_epoch<<"/"<<epochs;
    }//End of while
    if (show_progress)
        std::cout<<std::endl;
    isTrained = true;
    gradiente.clear();
}

void NeuralNet::trainSGD(std::vector< std::vector<double> >inputData, std::vector< std::vector<double> >outputData, unsigned int miniBatchSize)
{
    unsigned int layer, neuron, input, pos, i;
    if (!isConstructed)
    {
        return;
    }
    if (learnRate == 0 || epochs == 0)
    {
        return;
    }
    unsigned int inputSize = inputData.size();
    if (inputSize != outputData.size())
        return;

    unsigned int nMiniBatches = inputSize/miniBatchSize;

    gradiente.resize(nLayers-1);
    for (layer=0; layer<nLayers-1; layer++)
    {
        gradiente[layer].dw = std::vector< std::vector<double> >( layerSize[layer+1], std::vector<double>(layerSize[layer]) );
        gradiente[layer].db = std::vector<double>( layerSize[layer+1] );
        gradiente[layer].delta = std::vector<double>( layerSize[layer+1] );
    }

    std::vector<unsigned int> randomPos(inputSize);
    for (i=0; i<inputSize; i++)
        randomPos[i] = i;

    unsigned int cur_epoch = 0;
    while (cur_epoch++ < epochs)
    {
        for (layer = 0; layer<nLayers-1; layer++)
        {
            for (neuron = 0; neuron<layerSize[layer+1]; neuron++)
            {
                gradiente[layer].db[neuron] = 0;
                for (i = 0; i<layerSize[layer]; i++)
                    gradiente[layer].dw[neuron][i] = 0;
            }
        }
        ///Assigning Mini Batches
        std::random_shuffle(randomPos.begin(), randomPos.end());
        ///----

        for ( unsigned int cur_miniBatch=0; cur_miniBatch<nMiniBatches; cur_miniBatch++ )
        {

            for ( input = 0; input<miniBatchSize; input++)
            {
                /// Update Mini Batch
                pos = randomPos[cur_miniBatch*miniBatchSize + input];
                backpropagation( inputData[ pos ], outputData[ pos ]);
                for (layer = 0; layer<nLayers-1; layer++)
                {
                    for (neuron = 0; neuron<layerSize[layer+1]; neuron++)
                    {
                        if (layer==0)
                        {
                            for (i = 0; i<layerSize[layer]; i++)
                            gradiente[layer].dw[neuron][i] += inputData[pos][i] * gradiente[layer].delta[neuron];
                        }
                        else
                        {
                            for (i = 0; i<layerSize[layer]; i++)
                                gradiente[layer].dw[neuron][i] += rede[layer-1].activation[i] * gradiente[layer].delta[neuron];
                        }
                        gradiente[layer].db[neuron] += gradiente[layer].delta[neuron];
                    }
                }
            }
            for (layer = 0; layer<nLayers-1; layer++)
            {
                for (neuron = 0; neuron<layerSize[layer+1]; neuron++)
                {
                    rede[layer].bias[neuron] -= (learnRate/inputSize) * gradiente[layer].db[neuron];
                    for (i = 0; i<layerSize[layer]; i++)
                        rede[layer].weight[neuron][i] -= (learnRate/inputSize) * gradiente[layer].dw[neuron][i];
                }
            }
            ///Update Mini Batch - END
        }
        if (show_progress)
            std::cout<<"\r"<<cur_epoch<<"/"<<epochs;
        if (evaluate_net)
        {
            std::cout<<"Epoch "<<cur_epoch<<" - ";
            evaluate();
        }

    }//End of while
    if (show_progress || evaluate_net)
        std::cout<<std::endl;
    isTrained = true;
    gradiente.clear();
}

void NeuralNet::use(std::vector<double> input, std::vector<double> *output) ///YES
{
    if (!isConstructed)
        return;
    if (output == NULL)
        return;
    if (input.size()!=layerSize.front())
    {
        output->clear();
        return;
    }
    calculateOutput(input);
    *output = rede.back().activation;
}

void NeuralNet::evaluate()
{
    if (evaluation_input.empty() || evaluation_output.empty())
        return;
    double error = 0;
    unsigned int outputMaxIndex, desiredMaxIndex;
    unsigned int correctOutputs = 0;
    double outputMax, desiredMax;
    outputMaxIndex = desiredMaxIndex = 0;
    outputMax = desiredMax = 0;
    for (unsigned int input=0; input<evaluation_input.size(); input++)
    {
        calculateOutput(evaluation_input[input]);
        error += cost(evaluation_output[input], rede.back().activation);
        outputMaxIndex = desiredMaxIndex = 0;
        outputMax = desiredMax = 0;
        for (unsigned int i=0; i<layerSize.back(); i++)
        {
            if (evaluation_output[input][i] > desiredMax)
            {
                desiredMax = evaluation_output[input][i];
                desiredMaxIndex = i;
            }
            if (rede.back().activation[i] > outputMax)
            {
                outputMax = rede.back().activation[i];
                outputMaxIndex = i;
            }
        }
        if (desiredMaxIndex == outputMaxIndex)
            correctOutputs++;
    }

    std::cout<<"Acertos da rede: "<<correctOutputs<<"/"<<evaluation_input.size();
    std::cout<<"\tErro medio: "<<error/(double)evaluation_input.size()<<std::endl;
}

void NeuralNet::showWeightsBias() ///YES
{
    if (!isConstructed)
        return;
    for (unsigned int layer=0; layer<nLayers-1; layer++)
    {
        if (layer == 0)
        {
            std::cout<<"**Hidden Layer"<<std::endl;
            std::cout<<"---Layer "<<layer+1;
        }
        else if (layer<rede.size()-1)
            std::cout<<"---Layer "<<layer+1;
        else
            std::cout<<"**Output";
        for (unsigned int neuron=0; neuron<layerSize.at(layer+1); neuron++)
        {
            std::cout<<std::endl<<"    Neuronio "<<neuron+1<<"..."<<std::endl;
            std::cout<<"     >Bias = "<<rede.at(layer).bias[neuron]<<std::endl;
            for (unsigned int i=0; i<layerSize.at(layer); i++)
                std::cout<<"     >Peso "<<i+1<<" = "<<rede.at(layer).weight[neuron][i]<<std::endl;
        }
        std::cout<<std::endl;
    }
}

///Private Functions

void NeuralNet::generateWeightsBias() ///YES
{
    srand(time(NULL));
    for (unsigned int layer=0; layer<nLayers-1; layer++)
    {
        std::vector< std::vector<double> > weight_temp(layerSize.at(layer+1), std::vector<double>(layerSize.at(layer)) );
        std::vector<double> bias_temp( layerSize.at(layer+1) );
        for (unsigned int neuron=0; neuron<layerSize.at(layer+1); neuron++)
        {
            bias_temp[neuron] = (double)( (rand()%20)-10 )/10.0f;
            for (unsigned int i=0; i<layerSize.at(layer); i++)
                weight_temp[neuron][i] = (double)( (rand()%20)-10 )/10.0f;
        }
        rede.at(layer).bias = bias_temp;
        rede.at(layer).weight = weight_temp;
    }
}

void NeuralNet::calculateOutput(std::vector<double> input) ///YES
{
    for (unsigned int neuron=0; neuron<layerSize[1]; neuron++)
    {
        rede[0].net[neuron] = 0;
        for (unsigned int j=0; j<layerSize[0]; j++)
            rede[0].net[neuron] += rede[0].weight[neuron][j] * input[j];
        rede[0].net[neuron] += rede[0].bias[neuron];
        rede[0].activation[neuron] = activationFunction(rede[0].net[neuron]);
    }

    for (unsigned int layer=1; layer<nLayers-1; layer++)
    {
        for (unsigned int neuron=0; neuron<layerSize[layer+1]; neuron++)
        {
                rede[layer].net[neuron] = 0;
                for (unsigned int j=0; j<layerSize[layer]; j++)
                    rede[layer].net[neuron] += rede[layer].weight[neuron][j] * rede[layer-1].activation[j];
                rede[layer].net[neuron] += rede[layer].bias[neuron];
            rede[layer].activation[neuron] = activationFunction(rede[layer].net[neuron]);
        }
    }
    return;
}

void NeuralNet::backpropagation(std::vector<double> inputData, std::vector<double> outputData)
{
    unsigned int layer, neuron;
    calculateOutput(inputData);
    for (layer = nLayers-1; layer>0; layer--)
    {
        if (layer == nLayers-1)
        {
            for (neuron=0; neuron<layerSize[layer]; neuron++)
                gradiente[layer-1].delta[neuron] = cost_derivative(outputData[neuron], rede[layer-1].activation[neuron]) * derivativeActivationFunction( rede[layer-1].net[neuron] );
        }
        else
        {
            for (neuron=0; neuron<layerSize[layer]; neuron++)
            {
                gradiente[layer-1].delta[neuron] = 0;
                for (unsigned int j=0; j<layerSize[layer+1] ; j++)
                {
                    gradiente[layer-1].delta[neuron] += rede[layer].weight[j][neuron] * gradiente[layer].delta[j];
                }
                gradiente[layer-1].delta[neuron] *= derivativeActivationFunction( rede[layer-1].net[neuron] );
            }
        }
    }
}

void NeuralNet::showAll() ///YES
{
    std::cout<<"Layers Size ";
    for (unsigned int i=0; i<nLayers; i++)
        std::cout<<layerSize[i]<<" ";
    std::cout<<std::endl<<"NLayers: "<<nLayers<<std::endl;
    std::cout<<std::endl<<"Learning Rate: "<<learnRate<<std::endl;
    std::cout<<std::endl<<"Epochs: "<<epochs<<std::endl;
    std::cout<<std::endl<<"isTrained: "<<isTrained<<std::endl;
    std::cout<<std::endl<<"isConstructed: "<<isConstructed<<std::endl;
    getch();
}

#endif // NEURAL_NET_H_INCLUDED
