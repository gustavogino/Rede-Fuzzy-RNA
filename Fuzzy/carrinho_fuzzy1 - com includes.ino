class FuzzySet {
    public:
        // CONSTRUTORES
        FuzzySet();
        FuzzySet(float a, float b, float c, float d);
        // MÉTODOS PÚBLICOS
        float getPointA();
        float getPointB();
        float getPointC();
        float getPointD();
        bool calculatePertinence(float crispValue);
        void setPertinence(float pertinence);
        float getPertinence();
        void reset();

    private:
        // VARIÁVEIS
        float a;
        float b;
        float c;
        float d;
        float pertinence;
};

FuzzySet::FuzzySet(){
}

FuzzySet::FuzzySet(float a, float b, float c, float d){
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
    this->pertinence = 0.0;
}

float FuzzySet::getPointA(){
    return this->a;
}

float FuzzySet::getPointB(){
    return this->b;
}

float FuzzySet::getPointC(){
    return this->c;
}

float FuzzySet::getPointD(){
    return this->d;
}

bool FuzzySet::calculatePertinence(float crispValue){
    float slope;

    if (crispValue < this->a){
        if (this->a == this->b && this->b != this->c && this->c != this->d){
            this->pertinence = 1.0;
        }else{
            this->pertinence = 0.0;
        }
    }else if (crispValue >= this->a && crispValue < this->b){
        slope = 1.0 / (this->b - this->a);
        this->pertinence = slope * (crispValue - this->b) + 1.0;
    }else if (crispValue >= this->b && crispValue <= this->c){
        this->pertinence = 1.0;
    }else if (crispValue > this->c && crispValue <= this->d){
        slope = 1.0 / (this->c - this->d);
        this->pertinence = slope * (crispValue - this->c) + 1.0;
    }else if (crispValue > this->d){
        if (this->c == this->d && this->c != this->b && this->b != this->a){
            this->pertinence = 1.0;
        }else{
            this->pertinence = 0.0;
        }
    }
    return true;
}

void FuzzySet::setPertinence(float pertinence){
    if(this->pertinence < pertinence){
        this->pertinence = pertinence;
    }
}

float FuzzySet::getPertinence(){
    return this->pertinence;
}

void FuzzySet::reset(){
    this->pertinence = 0.0;
}

struct fuzzySetArray{
    FuzzySet* fuzzySet;
    fuzzySetArray* next;
};

class FuzzyIO {
    public:
        // CONSTRUTORES
        FuzzyIO();
        FuzzyIO(int index);
        // DESTRUTOR
        ~FuzzyIO();
        // MÉTODOS PÚBLICOS
        int getIndex();
        void setCrispInput(float crispInput);
        float getCrispInput();
        bool addFuzzySet(FuzzySet* fuzzySet);
        void resetFuzzySets();

    protected:
        // VARIÁVEIS PROTEGIDAS
        int index;
        float crispInput;
        fuzzySetArray* fuzzySets;
        fuzzySetArray* fuzzySetsCursor;
        // MÉTODOS PROTEGIDOS
        void cleanFuzzySets(fuzzySetArray* aux);
};

FuzzyIO::FuzzyIO(){
}

FuzzyIO::FuzzyIO(int index){
    this->index = index;
    // Iniciando os ponteiros como nulo
    this->fuzzySets          = NULL;
    this->fuzzySetsCursor    = NULL;
}

// DESTRUTOR
FuzzyIO::~FuzzyIO(){
    this->cleanFuzzySets(this->fuzzySets);
}

// MÉTODOS PÚBLICOS
int FuzzyIO::getIndex(){
    return this->index;
}

void FuzzyIO::setCrispInput(float crispInput){
    this->crispInput = crispInput;
}

float FuzzyIO::getCrispInput(){
    return this->crispInput;
}

bool FuzzyIO::addFuzzySet(FuzzySet* fuzzySet){
    fuzzySetArray *aux;
    // Alocando espaço na memória
    if((aux = (fuzzySetArray *) malloc(sizeof(fuzzySetArray))) == NULL){
        return false;
    }
    aux->fuzzySet     = fuzzySet;
    aux->next         = NULL;

    if(this->fuzzySets == NULL){
        this->fuzzySets = aux;
        this->fuzzySetsCursor = aux;
    }else{
        this->fuzzySetsCursor->next = aux;
        this->fuzzySetsCursor = aux;
    }
    return true;
}

void FuzzyIO::resetFuzzySets(){
    fuzzySetArray* fuzzySetsAux;
    fuzzySetsAux = this->fuzzySets;
    // Calculando as pertinências de totos os FuzzyInputs
    while(fuzzySetsAux != NULL){
        fuzzySetsAux->fuzzySet->reset();
        fuzzySetsAux = fuzzySetsAux->next;
    }
}

// MÉTODOS PROTEGIDOS
void FuzzyIO::cleanFuzzySets(fuzzySetArray *aux){
    if(aux != NULL){
        // Esvaziando a memória alocada
        this->cleanFuzzySets(aux->next);
        free(aux);
    }
}

#define EPS 1.0E-3

// Estrutura de uma lista para guardar os pontos
struct pointsArray{
    pointsArray* previous;
    float point;
    float pertinence;
    pointsArray* next;
};

class FuzzyComposition{
    public:
        // CONSTRUTORES
        FuzzyComposition();
        // DESTRUTOR
        ~FuzzyComposition();
        // MÉTODOS PÚBLICOS
        bool addPoint(float point, float pertinence);
        bool checkPoint(float point, float pertinence);
        bool build();
        float avaliate();
        bool empty();

    private:
        // VARIÁVEIS PRIVADAS
        pointsArray* pointsCursor;
        pointsArray* points;

        // MÉTODOS PRIVADOS
        void cleanPoints(pointsArray* aux);
        bool rebuild(pointsArray* aSegmentBegin, pointsArray* aSegmentEnd, pointsArray* bSegmentBegin, pointsArray* bSegmentEnd);
        bool rmvPoint(pointsArray* point);
};

// CONSTRUTORES
FuzzyComposition::FuzzyComposition(){
    this->pointsCursor     = NULL;
    this->points         = NULL;
}

// DESTRUTOR
FuzzyComposition::~FuzzyComposition(){
    this->cleanPoints(this->points);
}

bool FuzzyComposition::addPoint(float point, float pertinence){
    pointsArray* aux;
    // Alocando espaço na memória
    if((aux = (pointsArray* ) malloc(sizeof(pointsArray))) == NULL){
        return false;
    }
    aux->previous = NULL;
    aux->point = point;
    aux->pertinence = pertinence;
    aux->next = NULL;

    if(this->points == NULL){
        this->points = aux;
        this->pointsCursor  = aux;
    }else{
        aux->previous = this->pointsCursor;
        this->pointsCursor = aux;
        aux->previous->next = this->pointsCursor;
    }
    return true;
}

bool FuzzyComposition::checkPoint(float point, float pertinence){
    pointsArray* aux;
    aux = this->pointsCursor;
    while(aux != NULL){
        if(aux->point == point && aux->pertinence == pertinence){
            return true;
        }
        aux = aux->previous;
    }
    return false;
}

bool FuzzyComposition::build(){
    pointsArray* aux;

    aux = this->points;
    while(aux != NULL){
        pointsArray* temp = aux;
        while(temp->previous != NULL){
            if(temp->point < temp->previous->point){
                break;
            }
            temp = temp->previous;
        }
        pointsArray* zPoint;
        if(temp != NULL){
            zPoint = temp;
            while(temp->previous != NULL){
                bool result = false;
                if(temp->previous->previous != NULL){
                    result = rebuild(zPoint, zPoint->next, temp->previous, temp->previous->previous);
                }
                if(result == true){
                    aux = this->points;
                    break;
                }
                temp = temp->previous;
            }
        }
        aux = aux->next;
    }
    return true;
}

float FuzzyComposition::avaliate(){
    pointsArray* aux;
    float numerator     = 0.0;
    float denominator   = 0.0;

    aux = this->points;
    while(aux != NULL){
        if(aux->next != NULL){
            float area = 0.0;
            float middle = 0.0;
            if(aux->point == aux->next->point){
                // Se Singleton
                area     = aux->pertinence;
                middle   = aux->point;
            }else if(aux->pertinence == 0.0 || aux->next->pertinence == 0.0){
                // Se triangulo
                float pertinence;
                if(aux->pertinence > 0.0){
                    pertinence = aux->pertinence;
                }else{
                    pertinence = aux->next->pertinence;
                }
                area = ((aux->next->point - aux->point) * pertinence) / 2.0;
                if(aux->pertinence < aux->next->pertinence){
                    middle = ((aux->next->point - aux->point) / 1.5) + aux->point;
                }else{
                    middle = ((aux->next->point - aux->point) / 3.0) + aux->point;
                }
            }else if((aux->pertinence > 0.0 && aux->next->pertinence > 0.0) && (aux->pertinence == aux->next->pertinence)){
                // Se quadrado
                area = (aux->next->point - aux->point) * aux->pertinence;
                middle = ((aux->next->point - aux->point) / 2.0) + aux->point;
            }else if((aux->pertinence > 0.0 && aux->next->pertinence > 0.0) && (aux->pertinence != aux->next->pertinence)){
                // Se trapezio
                area = ((aux->pertinence + aux->next->pertinence) / 2.0) * (aux->next->point - aux->point);
                middle = ((aux->next->point - aux->point) / 2.0) + aux->point;
            }
            numerator += middle * area;
            denominator += area;
        }
        aux = aux->next;
    }

    if(denominator == 0.0){
        return 0.0;
    }else{
        return numerator / denominator;
    }
}

bool FuzzyComposition::empty(){
    // limpando a memória
    this->cleanPoints(this->points);
    // resetando os ponteiros
    this->points = NULL;
    this->pointsCursor = NULL;
    return true;
}

// MÉTODOS PRIVADOS
void FuzzyComposition::cleanPoints(pointsArray* aux){
    if(aux != NULL){
        // Esvaziando a memória alocada
        this->cleanPoints(aux->next);
        free(aux);
    }
}

bool FuzzyComposition::rebuild(pointsArray* aSegmentBegin, pointsArray* aSegmentEnd, pointsArray* bSegmentBegin, pointsArray* bSegmentEnd){
    float x1 = aSegmentBegin->point;
    float y1 = aSegmentBegin->pertinence;
    float x2 = aSegmentEnd->point;
    float y2 = aSegmentEnd->pertinence;
    float x3 = bSegmentBegin->point;
    float y3 = bSegmentBegin->pertinence;
    float x4 = bSegmentEnd->point;
    float y4 = bSegmentEnd->pertinence;
    float point, pertinence;
    float denom, numera, numerb;
    float mua, mub;

    denom  = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
    numera = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
    numerb = (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3);

    if(denom < 0.0){
        denom *= -1.0;
    }
    if(numera < 0.0){
        numera *= -1.0;
    }
    if(numerb < 0.0){
        numerb *= -1.0;
    }

    // Se os seguimentos forem paralelos, retornar falso
    if(denom < EPS){
        return false;
    }

    // Verificar se há interseção ao longo do seguimento
    mua = numera / denom;
    mub = numerb / denom;
    if(mua < 0.0 || mua > 1.0 || mub < 0.0 || mub > 1.0){
        return false;
    }else{
        // Calculando o ponto e a pertinencia do novo elemento
        point         = x1 + mua * (x2 - x1);
        pertinence     = y1 + mua * (y2 - y1);

        // Adicionando um novo ponto
        pointsArray* aux;
        // Alocando espaço na memória
        if((aux = (pointsArray *) malloc(sizeof(pointsArray))) == NULL){
            return false;
        }

        aux->previous = bSegmentEnd;
        aux->point = point;
        aux->pertinence = pertinence;
        aux->next = aSegmentEnd;

        bSegmentEnd->next = aux;
        aSegmentEnd->previous = aux;

        float stopPoint = bSegmentBegin->point;
        float stopPertinence = bSegmentBegin->pertinence;

        pointsArray* temp = aSegmentBegin;
        pointsArray* excl;

        do{
            float pointToCompare = temp->point;
            float pertinenceToCompare = temp->pertinence;

            excl = temp->previous;

            this->rmvPoint(temp);

            temp = excl;

            if(stopPoint == pointToCompare && stopPertinence == pertinenceToCompare){
                break;
            }
        }while(temp != NULL);

        return true;
    }
}

bool FuzzyComposition::rmvPoint(pointsArray* point){
    if(point != NULL){
        free(point);
    }
    return true;
}

// Estrutura de uma linha
struct line{
    float xBegin;
    float yBegin;
    float xEnd;
    float yEnd;
};

class FuzzyOutput : public FuzzyIO {
    public:
        // CONSTRUTORES
        FuzzyOutput();
        FuzzyOutput(int index);
        // DESTRUTOR
        ~FuzzyOutput();
        // MÉTODOS PÚBLICOS
        bool truncate();
        float getCrispOutput();
        bool order();

    private:
        // VARIÁVEIS PRIVADAS
        FuzzyComposition fuzzyComposition;
        // MÉTODOS PRIVADOS
        bool swap(fuzzySetArray* fuzzySetA, fuzzySetArray* fuzzySetB);
        bool rebuild(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float* point, float* pertinence);
};

// CONSTRUTORES
FuzzyOutput::FuzzyOutput() : FuzzyIO(){
}

FuzzyOutput::FuzzyOutput(int index) : FuzzyIO(index){
}

// DESTRUTOR
FuzzyOutput::~FuzzyOutput(){
    this->fuzzyComposition.empty();
}

// MÉTODOS PÚBLICOS
bool FuzzyOutput::truncate(){
    // esvaziando a composição
    this->fuzzyComposition.empty();

    fuzzySetArray *aux;
    aux = this->fuzzySets;
    while(aux != NULL){
        if(aux->fuzzySet->getPertinence() > 0.0){
            // Se não for trapezio iniciado com pertinencia 1 (sem o triangulo esquerdo)
            if(aux->fuzzySet->getPointA() != aux->fuzzySet->getPointB()){
                if(this->fuzzyComposition.checkPoint(aux->fuzzySet->getPointA(), 0.0) == false){
                    this->fuzzyComposition.addPoint(aux->fuzzySet->getPointA(), 0.0);
                }
            }

            if(aux->fuzzySet->getPointB() == aux->fuzzySet->getPointC() && aux->fuzzySet->getPointA() != aux->fuzzySet->getPointD()){
                // se trinagulo
                if(aux->fuzzySet->getPertinence() == 1.0){
                    if(this->fuzzyComposition.checkPoint(aux->fuzzySet->getPointB(), aux->fuzzySet->getPertinence()) == false){
                        this->fuzzyComposition.addPoint(aux->fuzzySet->getPointB(), aux->fuzzySet->getPertinence());
                    }
                }else{
                    float newPointB         = aux->fuzzySet->getPointB();
                    float newPertinenceB    = aux->fuzzySet->getPertinence();

                    rebuild(aux->fuzzySet->getPointA(), 0.0, aux->fuzzySet->getPointB(), 1.0, aux->fuzzySet->getPointA(), aux->fuzzySet->getPertinence(), aux->fuzzySet->getPointD(), aux->fuzzySet->getPertinence(), &newPointB, &newPertinenceB);

                    if(this->fuzzyComposition.checkPoint(newPointB, newPertinenceB) == false){
                        this->fuzzyComposition.addPoint(newPointB, newPertinenceB);
                    }

                    float newPointC         = aux->fuzzySet->getPointB();
                    float newPertinenceC    = aux->fuzzySet->getPertinence();

                    rebuild(aux->fuzzySet->getPointC(), 1.0, aux->fuzzySet->getPointD(), 0.0, aux->fuzzySet->getPointA(), aux->fuzzySet->getPertinence(), aux->fuzzySet->getPointD(), aux->fuzzySet->getPertinence(), &newPointC, &newPertinenceC);

                    if(this->fuzzyComposition.checkPoint(newPointC, newPertinenceC) == false){
                        this->fuzzyComposition.addPoint(newPointC, newPertinenceC);
                    }
                }
            }else if(aux->fuzzySet->getPointB() != aux->fuzzySet->getPointC()){
                // se trapezio
                if(aux->fuzzySet->getPertinence() == 1.0){
                    if(this->fuzzyComposition.checkPoint(aux->fuzzySet->getPointB(), aux->fuzzySet->getPertinence()) == false){
                        this->fuzzyComposition.addPoint(aux->fuzzySet->getPointB(), aux->fuzzySet->getPertinence());
                    }

                    if(this->fuzzyComposition.checkPoint(aux->fuzzySet->getPointC(), aux->fuzzySet->getPertinence()) == false){
                        this->fuzzyComposition.addPoint(aux->fuzzySet->getPointC(), aux->fuzzySet->getPertinence());
                    }
                }else{
                    float newPointB         = aux->fuzzySet->getPointB();
                    float newPertinenceB    = aux->fuzzySet->getPertinence();

                    rebuild(aux->fuzzySet->getPointA(), 0.0, aux->fuzzySet->getPointB(), 1.0, aux->fuzzySet->getPointA(), aux->fuzzySet->getPertinence(), aux->fuzzySet->getPointD(), aux->fuzzySet->getPertinence(), &newPointB, &newPertinenceB);

                    if(this->fuzzyComposition.checkPoint(newPointB, newPertinenceB) == false){
                        this->fuzzyComposition.addPoint(newPointB, newPertinenceB);
                    }

                    float newPointC         = aux->fuzzySet->getPointB();
                    float newPertinenceC    = aux->fuzzySet->getPertinence();

                    rebuild(aux->fuzzySet->getPointC(), 1.0, aux->fuzzySet->getPointD(), 0.0, aux->fuzzySet->getPointA(), aux->fuzzySet->getPertinence(), aux->fuzzySet->getPointD(), aux->fuzzySet->getPertinence(), &newPointC, &newPertinenceC);

                    if(this->fuzzyComposition.checkPoint(newPointC, newPertinenceC) == false){
                        this->fuzzyComposition.addPoint(newPointC, newPertinenceC);
                    }
                }
            }else{
                //senao singleton
                if(this->fuzzyComposition.checkPoint(aux->fuzzySet->getPointB(), aux->fuzzySet->getPertinence()) == false){
                    this->fuzzyComposition.addPoint(aux->fuzzySet->getPointB(), aux->fuzzySet->getPertinence());
                }
            }
            
            //Se não for trapezio iniciado com pertinencia 1 (sem o triangulo direito)
            if(aux->fuzzySet->getPointC() != aux->fuzzySet->getPointD()){
                if(this->fuzzyComposition.checkPoint(aux->fuzzySet->getPointD(), 0.0) == false || aux->fuzzySet->getPointD() == aux->fuzzySet->getPointA()){
                    this->fuzzyComposition.addPoint(aux->fuzzySet->getPointD(), 0.0);
                }
            }
        }
        aux = aux->next;
    }

    this->fuzzyComposition.build();

    return true;
}

float FuzzyOutput::getCrispOutput(){
    return this->fuzzyComposition.avaliate();
}

// Um simples Bubble Sort
bool FuzzyOutput::order(){
    fuzzySetArray *aux1;
    fuzzySetArray *aux2;

    aux1 = this->fuzzySets;
    aux2 = this->fuzzySets;

    while(aux1 != NULL){
        while(aux2 != NULL){
            if(aux2->next != NULL){
                if(aux2->fuzzySet->getPointA() > aux2->next->fuzzySet->getPointA()){
                    this->swap(aux2, aux2->next);
                }
            }
            aux2 = aux2->next;
        }
        aux2 = this->fuzzySets;
        aux1 = aux1->next;
    }
    return true;
}

// MÉTODOS PRIVADOS
bool FuzzyOutput::swap(fuzzySetArray* fuzzySetA, fuzzySetArray* fuzzySetB){
    FuzzySet* aux;
    
    aux = fuzzySetA->fuzzySet;
    fuzzySetA->fuzzySet = fuzzySetB->fuzzySet;
    fuzzySetB->fuzzySet = aux;

    return true;
}

bool FuzzyOutput::rebuild(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float* point, float* pertinence){
    float denom, numera, numerb;
    float mua, mub;

    denom  = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
    numera = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
    numerb = (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3);

    if(denom < 0.0){
        denom *= -1.0;
    }
    if(numera < 0.0){
        numera *= -1.0;
    }
    if(numerb < 0.0){
        numerb *= -1.0;
    }

    // Se os seguimentos forem paralelos, retornar falso
    if(denom < EPS){
        return false;
    }

    // Verificar se há interseção ao longo do seguimento
    mua = numera / denom;
    mub = numerb / denom;
    if(mua < 0.0 || mua > 1.0 || mub < 0.0 || mub > 1.0){
        return false;
    }else{
        // Calculando o ponto e a pertinencia do novo elemento
        *point      = x1 + mua * (x2 - x1);
        *pertinence = y1 + mua * (y2 - y1);

        return true;
    }
}

class FuzzyInput : public FuzzyIO {
    public:
        // CONSTRUTORES
        FuzzyInput();
        FuzzyInput(int index);
        // DESTRUTOR
        ~FuzzyInput();
        // MÉTODOS PÚBLICOS
        bool calculateFuzzySetPertinences();
};

// CONSTRUTORES
FuzzyInput::FuzzyInput() : FuzzyIO(){
}

FuzzyInput::FuzzyInput(int index) : FuzzyIO(index){
}

// DESTRUTOR
FuzzyInput::~FuzzyInput(){
}

// MÉTODOS PÚBLICOS
bool FuzzyInput::calculateFuzzySetPertinences(){
    fuzzySetArray *aux;
    aux = this->fuzzySets;

    while(aux != NULL){
        if (aux->fuzzySet != NULL){
            aux->fuzzySet->calculatePertinence(this->crispInput);
        }
        aux = aux->next;
    }
    
    return true;
}

// CONSTANTES
#define OP_AND 1
#define OP_OR 2
#define MODE_FS 1
#define MODE_FS_FS 2
#define MODE_FS_FRA 3
#define MODE_FRA_FRA 4

class FuzzyRuleAntecedent {
    public:
        // CONSTRUTORES
        FuzzyRuleAntecedent();
        // MÉTODOS PÚBLICOS
        bool joinSingle(FuzzySet* fuzzySet);
        bool joinWithAND(FuzzySet* fuzzySet1, FuzzySet* fuzzySet2);
        bool joinWithOR(FuzzySet* fuzzySet1, FuzzySet* fuzzySet2);
        bool joinWithAND(FuzzySet* fuzzySet, FuzzyRuleAntecedent* fuzzyRuleAntecedent);
        bool joinWithAND(FuzzyRuleAntecedent* fuzzyRuleAntecedent, FuzzySet* fuzzySet);
        bool joinWithOR(FuzzySet* fuzzySet, FuzzyRuleAntecedent* fuzzyRuleAntecedent);
        bool joinWithOR(FuzzyRuleAntecedent* fuzzyRuleAntecedent, FuzzySet* fuzzySet);
        bool joinWithAND(FuzzyRuleAntecedent* fuzzyRuleAntecedent1, FuzzyRuleAntecedent* fuzzyRuleAntecedent2);
        bool joinWithOR(FuzzyRuleAntecedent* fuzzyRuleAntecedent1, FuzzyRuleAntecedent* fuzzyRuleAntecedent2);
        float evaluate();

    private:
        // VARIÁVEIS PRIVADAS
        int op; // operador lógico
        int mode;
        FuzzySet* fuzzySet1;
        FuzzySet* fuzzySet2;
        FuzzyRuleAntecedent* fuzzyRuleAntecedent1;
        FuzzyRuleAntecedent* fuzzyRuleAntecedent2;
};

// CONSTRUTORES
FuzzyRuleAntecedent::FuzzyRuleAntecedent(){
    this->op = 0;
    this->mode = 0;
    this->fuzzySet1 = NULL;
    this->fuzzySet2 = NULL;
    this->fuzzyRuleAntecedent1 = NULL;
    this->fuzzyRuleAntecedent2 = NULL;
}

// MÉTODOS PÚBLICOS
bool FuzzyRuleAntecedent::joinSingle(FuzzySet* fuzzySet){
    if(fuzzySet){
        this->mode = MODE_FS;
        this->fuzzySet1 = fuzzySet;
        return true;
    }
    return false;
}

bool FuzzyRuleAntecedent::joinWithAND(FuzzySet* fuzzySet1, FuzzySet* fuzzySet2){
    if(fuzzySet1 != NULL && fuzzySet2 != NULL){
        this->op = OP_AND;
        this->mode = MODE_FS_FS;
        this->fuzzySet1 = fuzzySet1;
        this->fuzzySet2 = fuzzySet2;
        return true;
    }
    return false;
}

bool FuzzyRuleAntecedent::joinWithOR(FuzzySet* fuzzySet1, FuzzySet* fuzzySet2){
    if(fuzzySet1 != NULL && fuzzySet2 != NULL){
        this->op = OP_OR;
        this->mode = MODE_FS_FS;
        this->fuzzySet1 = fuzzySet1;
        this->fuzzySet2 = fuzzySet2;
        return true;
    }
    return false;
}

bool FuzzyRuleAntecedent::joinWithAND(FuzzySet* fuzzySet, FuzzyRuleAntecedent* fuzzyRuleAntecedent){
    if(fuzzySet != NULL && fuzzyRuleAntecedent != NULL){
        this->op = OP_AND;
        this->mode = MODE_FS_FRA;
        this->fuzzySet1 = fuzzySet;
        this->fuzzyRuleAntecedent1 = fuzzyRuleAntecedent;
        return true;
    }
    return false;
}

bool FuzzyRuleAntecedent::joinWithAND(FuzzyRuleAntecedent* fuzzyRuleAntecedent, FuzzySet* fuzzySet){
    if(fuzzySet != NULL && fuzzyRuleAntecedent != NULL){
        this->op = OP_AND;
        this->mode = MODE_FS_FRA;
        this->fuzzySet1 = fuzzySet;
        this->fuzzyRuleAntecedent1 = fuzzyRuleAntecedent;
        return true;
    }
    return false;
}

bool FuzzyRuleAntecedent::joinWithOR(FuzzySet* fuzzySet, FuzzyRuleAntecedent* fuzzyRuleAntecedent){
    if(fuzzySet != NULL && fuzzyRuleAntecedent != NULL){
        this->op = OP_OR;
        this->mode = MODE_FS_FRA;
        this->fuzzySet1 = fuzzySet;
        this->fuzzyRuleAntecedent1 = fuzzyRuleAntecedent;
        return true;
    }
    return false;
}

bool FuzzyRuleAntecedent::joinWithOR(FuzzyRuleAntecedent* fuzzyRuleAntecedent, FuzzySet* fuzzySet){
    if(fuzzySet != NULL && fuzzyRuleAntecedent != NULL){
        this->op = OP_OR;
        this->mode = MODE_FS_FRA;
        this->fuzzySet1 = fuzzySet;
        this->fuzzyRuleAntecedent1 = fuzzyRuleAntecedent;
        return true;
    }
    return false;
}

bool FuzzyRuleAntecedent::joinWithAND(FuzzyRuleAntecedent* fuzzyRuleAntecedent1, FuzzyRuleAntecedent* fuzzyRuleAntecedent2){
    if(fuzzyRuleAntecedent1 != NULL && fuzzyRuleAntecedent2 != NULL){
        this->op = OP_AND;
        this->mode = MODE_FRA_FRA;
        this->fuzzyRuleAntecedent1 = fuzzyRuleAntecedent1;
        this->fuzzyRuleAntecedent2 = fuzzyRuleAntecedent2;
        return true;
    }
    return false;
}

bool FuzzyRuleAntecedent::joinWithOR(FuzzyRuleAntecedent* fuzzyRuleAntecedent1, FuzzyRuleAntecedent* fuzzyRuleAntecedent2){
    if(fuzzyRuleAntecedent1 != NULL && fuzzyRuleAntecedent2 != NULL){
        this->op = OP_OR;
        this->mode = MODE_FRA_FRA;
        this->fuzzyRuleAntecedent1 = fuzzyRuleAntecedent1;
        this->fuzzyRuleAntecedent2 = fuzzyRuleAntecedent2;
        return true;
    }
    return false;
}

float FuzzyRuleAntecedent::evaluate(){
    switch(this->mode){
        case MODE_FS:
            return this->fuzzySet1->getPertinence();
            break;
        case MODE_FS_FS:
            switch(this->op){
                case OP_AND:
                    if(this->fuzzySet1->getPertinence() > 0.0 && this->fuzzySet2->getPertinence() > 0.0){
                        if(this->fuzzySet1->getPertinence() < this->fuzzySet2->getPertinence()){
                            return this->fuzzySet1->getPertinence();
                        }else{
                            return this->fuzzySet2->getPertinence();
                        }
                    }else{
                        return 0.0;
                    }
                    break;
                case OP_OR:
                    if(this->fuzzySet1->getPertinence() > 0.0 || this->fuzzySet2->getPertinence() > 0.0){
                        if(this->fuzzySet1->getPertinence() > this->fuzzySet2->getPertinence()){
                            return this->fuzzySet1->getPertinence();
                        }else{
                            return this->fuzzySet2->getPertinence();
                        }
                    }else{
                        return 0.0;
                    }
                    break;
                default:
                    return 0.0;
            }
            break;
        case MODE_FS_FRA:
            switch(this->op){
                case OP_AND:
                    if(this->fuzzySet1->getPertinence() > 0.0 && fuzzyRuleAntecedent1->evaluate() > 0.0){
                        if(this->fuzzySet1->getPertinence() < fuzzyRuleAntecedent1->evaluate()){
                            return this->fuzzySet1->getPertinence();
                        }else{
                            return fuzzyRuleAntecedent1->evaluate();
                        }
                    }else{
                        return 0.0;
                    }
                    break;
                case OP_OR:
                    if(this->fuzzySet1->getPertinence() > 0.0 || fuzzyRuleAntecedent1->evaluate() > 0.0){
                        if(this->fuzzySet1->getPertinence() > fuzzyRuleAntecedent1->evaluate()){
                            return this->fuzzySet1->getPertinence();
                        }else{
                            return fuzzyRuleAntecedent1->evaluate();
                        }
                    }else{
                        return 0.0;
                    }
                    break;
                default:
                    return 0.0;
            }
            break;
        case MODE_FRA_FRA:
            switch(this->op){
                case OP_AND:
                    if(fuzzyRuleAntecedent1->evaluate() > 0.0 && fuzzyRuleAntecedent2->evaluate() > 0.0){
                        if(fuzzyRuleAntecedent1->evaluate() < fuzzyRuleAntecedent2->evaluate()){
                            return fuzzyRuleAntecedent1->evaluate();
                        }else{
                            return fuzzyRuleAntecedent2->evaluate();
                        }
                    }else{
                        return 0.0;
                    }
                    break;
                case OP_OR:
                    if(fuzzyRuleAntecedent1->evaluate() > 0.0 || fuzzyRuleAntecedent2->evaluate() > 0.0){
                        if(fuzzyRuleAntecedent1->evaluate() > fuzzyRuleAntecedent2->evaluate()){
                            return fuzzyRuleAntecedent1->evaluate();
                        }else{
                            return fuzzyRuleAntecedent2->evaluate();
                        }
                    }else{
                        return 0.0;
                    }
                    break;
                default:
                    return 0.0;
            }
            break;
        default:
            return 0.0;
    }
    return 0.0;
}

// Estrutura de uma lista de FuzzySet
struct fuzzySetOutputArray{
    FuzzySet* fuzzySet;
    fuzzySetOutputArray* next;
};

class FuzzyRuleConsequent {
    public:
        // CONSTRUTORES
        FuzzyRuleConsequent();
        // DESTRUTOR
        ~FuzzyRuleConsequent();
        // MÉTODOS PÚBLICOS
        bool addOutput(FuzzySet* fuzzySet);
        bool evaluate(float power);

    private:
        // VARIÁVEIS PRIVADAS
        fuzzySetOutputArray* fuzzySetOutputsCursor;
        fuzzySetOutputArray* fuzzySetOutputs;
        // MÉTODOS PRIVADOS
        void cleanFuzzySets(fuzzySetOutputArray* aux);
};

// CONSTRUTORES
FuzzyRuleConsequent::FuzzyRuleConsequent(){
    this->fuzzySetOutputs = NULL;
    this->fuzzySetOutputsCursor = NULL;
}

// DESTRUTOR
FuzzyRuleConsequent::~FuzzyRuleConsequent(){
    this->cleanFuzzySets(this->fuzzySetOutputs);
}

// MÉTODOS PÚBLICOS
bool FuzzyRuleConsequent::addOutput(FuzzySet* fuzzySet){
    fuzzySetOutputArray *aux;
    // Alocando espaço na memória
    if((aux = (fuzzySetOutputArray *) malloc(sizeof(fuzzySetOutputArray))) == NULL){
        return false;
    }
    aux->fuzzySet     = fuzzySet;
    aux->next         = NULL;

    if(this->fuzzySetOutputs == NULL){
        this->fuzzySetOutputs = aux;
        this->fuzzySetOutputsCursor    = aux;
    }else{
        this->fuzzySetOutputsCursor->next = aux;
        this->fuzzySetOutputsCursor = aux;
    }
    return true;
}

bool FuzzyRuleConsequent::evaluate(float power){
    fuzzySetOutputArray *aux;
    aux = this->fuzzySetOutputs;
    while(aux != NULL){
        aux->fuzzySet->setPertinence(power);
        aux = aux->next;
    }
    return true;
}

// MÉTODOS PRIVADOS
void FuzzyRuleConsequent::cleanFuzzySets(fuzzySetOutputArray* aux){
    if(aux != NULL){
        // Esvaziando a memória alocada
        this->cleanFuzzySets(aux->next);
        free(aux);
    }
}

class FuzzyRule {
    public:
        // CONSTRUTORES
        FuzzyRule();
        FuzzyRule(int index, FuzzyRuleAntecedent* fuzzyRuleAntecedent, FuzzyRuleConsequent* fuzzyRuleConsequent);
        // MÉTODOS PÚBLICOS
        int getIndex();
        bool evaluateExpression();
        bool isFired();

    private:
        // VARIÁVEIS PRIVADAS
        int index;
        bool fired;
        FuzzyRuleAntecedent* fuzzyRuleAntecedent;
        FuzzyRuleConsequent* fuzzyRuleConsequent;
};

FuzzyRule::FuzzyRule(){
}

FuzzyRule::FuzzyRule(int index, FuzzyRuleAntecedent* fuzzyRuleAntecedent, FuzzyRuleConsequent* fuzzyRuleConsequent){
    this->index = index;
    this->fuzzyRuleAntecedent = fuzzyRuleAntecedent;
    this->fuzzyRuleConsequent = fuzzyRuleConsequent;
    this->fired = false;
}

int FuzzyRule::getIndex(){
    return this->index;
}

bool FuzzyRule::evaluateExpression(){
    if (this->fuzzyRuleAntecedent != NULL){
        float powerOfAntecedent = this->fuzzyRuleAntecedent->evaluate();

        (powerOfAntecedent > 0.0) ?    (this->fired = true) : (this->fired = false);
        
        this->fuzzyRuleConsequent->evaluate(powerOfAntecedent);
    }
    return this->fired;
}

bool FuzzyRule::isFired(){
    return this->fired;
}

// Estrutura de uma matriz de fuzzyInputArray
struct fuzzyInputArray{
    FuzzyInput* fuzzyInput;
    fuzzyInputArray* next;
};

// Estrutura de uma matriz de fuzzyOutputArray
struct fuzzyOutputArray{
    FuzzyOutput* fuzzyOutput;
    fuzzyOutputArray* next;
};

// Estrutura de uma lista de FuzzyRule
struct fuzzyRuleArray{
    FuzzyRule* fuzzyRule;
    fuzzyRuleArray* next;
};

class Fuzzy {
    public:
        // CONSTRUTORES
        Fuzzy();
        // DESTRUTOR
        ~Fuzzy();
        // MÉTODOS PÚBLICOS
        bool addFuzzyInput(FuzzyInput* fuzzyInput);
        bool addFuzzyOutput(FuzzyOutput* fuzzyOutput);
        bool addFuzzyRule(FuzzyRule* fuzzyRule);
        bool setInput(int fuzzyInputIndex, float crispValue);
        bool fuzzify();
        bool isFiredRule(int fuzzyRuleIndex);
        float defuzzify(int fuzzyOutputIndex);

    private:
        // VARIÁVEIS PRIVADAS
        // ponteiros para gerenciar os arrays de FuzzyInput
        fuzzyInputArray* fuzzyInputsCursor;
        fuzzyInputArray* fuzzyInputs;
        // ponteiros para gerenciar os arrays de FuzzyOutput
        fuzzyOutputArray* fuzzyOutputsCursor;
        fuzzyOutputArray* fuzzyOutputs;
        // ponteiros para gerenciar os arrays de FuzzyRule
        fuzzyRuleArray* fuzzyRulesCursor;
        fuzzyRuleArray* fuzzyRules;

        // MÉTODOS PRIVADOS
        void cleanFuzzyInputs(fuzzyInputArray* aux);
        void cleanFuzzyOutputs(fuzzyOutputArray* aux);
        void cleanFuzzyRules(fuzzyRuleArray* aux);
};

// CONSTRUTORES
Fuzzy::Fuzzy(){
    // Iniciando os ponteiros como nulo
    // FuzzyInput
    this->fuzzyInputs       = NULL;
    this->fuzzyInputsCursor = NULL;
    // FuzzyOutput
    this->fuzzyOutputs          = NULL;
    this->fuzzyOutputsCursor    = NULL;
    // FuzzyRule
    this->fuzzyRules        = NULL;
    this->fuzzyRulesCursor  = NULL;
}

// DESTRUTOR
Fuzzy::~Fuzzy(){
    this->cleanFuzzyInputs(this->fuzzyInputs);
    this->cleanFuzzyOutputs(this->fuzzyOutputs);
    this->cleanFuzzyRules(this->fuzzyRules);
}

// MÉTODOS PÚBLICOS
bool Fuzzy::addFuzzyInput(FuzzyInput* fuzzyInput){
    fuzzyInputArray* aux;
    
    // Alocando espaço na memória
    if((aux = (fuzzyInputArray *) malloc(sizeof(fuzzyInputArray))) == NULL){
        return false;
    }

    aux->fuzzyInput = fuzzyInput;
    aux->next = NULL;

    if(this->fuzzyInputs == NULL){
        this->fuzzyInputs = aux;
        this->fuzzyInputsCursor  = aux;
    }else{
        this->fuzzyInputsCursor->next = aux;
        this->fuzzyInputsCursor = aux;
    }

    return true;
}

bool Fuzzy::addFuzzyOutput(FuzzyOutput* fuzzyOutput){
    fuzzyOutputArray *aux;
    // Alocando espaço na memória
    if((aux = (fuzzyOutputArray *) malloc(sizeof(fuzzyOutputArray))) == NULL){
        return false;
    }
    aux->fuzzyOutput = fuzzyOutput;
    aux->next = NULL;

    // Ordenando o fuzzyOutput
    fuzzyOutput->order();

    if(this->fuzzyOutputs == NULL){
        this->fuzzyOutputs = aux;
        this->fuzzyOutputsCursor  = aux;
    }else{
        this->fuzzyOutputsCursor->next = aux;
        this->fuzzyOutputsCursor = aux;
    }
    return true;
}

bool Fuzzy::addFuzzyRule(FuzzyRule* fuzzyRule){
    fuzzyRuleArray* aux;
    // Alocando espaço na memória
    if((aux = (fuzzyRuleArray *) malloc(sizeof(fuzzyRuleArray))) == NULL){
        return false;
    }
    aux->fuzzyRule = fuzzyRule;
    aux->next = NULL;

    if(this->fuzzyRules == NULL){
        this->fuzzyRules = aux;
        this->fuzzyRulesCursor  = aux;
    }else{
        this->fuzzyRulesCursor->next = aux;
        this->fuzzyRulesCursor = aux;
    }
    return true;
}

bool Fuzzy::setInput(int fuzzyInputIndex, float crispValue){
    fuzzyInputArray *aux;
    aux = this->fuzzyInputs;

    while(aux != NULL){
        if(aux->fuzzyInput->getIndex() == fuzzyInputIndex){
            aux->fuzzyInput->setCrispInput(crispValue);
            return true;
        }
        aux = aux->next;
    }
    return false;
}

bool Fuzzy::fuzzify(){
    fuzzyInputArray* fuzzyInputAux;

    fuzzyOutputArray *fuzzyOutputAux;

    fuzzyInputAux = this->fuzzyInputs;
    while(fuzzyInputAux != NULL){
        fuzzyInputAux->fuzzyInput->resetFuzzySets();
        fuzzyInputAux = fuzzyInputAux->next;
    }

    fuzzyOutputAux = this->fuzzyOutputs;
    while(fuzzyOutputAux != NULL){
        fuzzyOutputAux->fuzzyOutput->resetFuzzySets();
        fuzzyOutputAux = fuzzyOutputAux->next;
    }

    // Calculando a pertinência de todos os FuzzyInputs
    fuzzyInputAux = this->fuzzyInputs;
    while(fuzzyInputAux != NULL){
        fuzzyInputAux->fuzzyInput->calculateFuzzySetPertinences();
        fuzzyInputAux = fuzzyInputAux->next;
    }

    // Avaliando quais regras foram disparadas
    fuzzyRuleArray* fuzzyRuleAux;
    fuzzyRuleAux = this->fuzzyRules;
    // Calculando as pertinências de totos os FuzzyInputs
    while(fuzzyRuleAux != NULL){
        fuzzyRuleAux->fuzzyRule->evaluateExpression();
        fuzzyRuleAux = fuzzyRuleAux->next;
    }

    // Truncado os conjuntos de saída
    fuzzyOutputAux = this->fuzzyOutputs;
    while(fuzzyOutputAux != NULL){
        fuzzyOutputAux->fuzzyOutput->truncate();
        fuzzyOutputAux = fuzzyOutputAux->next;
    }

    return true;
}

bool Fuzzy::isFiredRule(int fuzzyRuleIndex){
    fuzzyRuleArray *aux;
    aux = this->fuzzyRules;
    while(aux != NULL){
        if(aux->fuzzyRule->getIndex() == fuzzyRuleIndex){
            return aux->fuzzyRule->isFired();
        }
        aux = aux->next;
    }
    return false;
}

float Fuzzy::defuzzify(int fuzzyOutputIndex){
    fuzzyOutputArray *aux;
    aux = this->fuzzyOutputs;
    while(aux != NULL){
        if(aux->fuzzyOutput->getIndex() == fuzzyOutputIndex){
            return aux->fuzzyOutput->getCrispOutput();
        }
        aux = aux->next;
    }
    return 0;
}

// MÉTODOS PRIVADOS
void Fuzzy::cleanFuzzyInputs(fuzzyInputArray* aux){
    if(aux != NULL){
        // Esvaziando a memória alocada
        this->cleanFuzzyInputs(aux->next);
        free(aux);
    }
}

void Fuzzy::cleanFuzzyOutputs(fuzzyOutputArray* aux){
    if(aux != NULL){
        // Esvaziando a memória alocada
        this->cleanFuzzyOutputs(aux->next);
        free(aux);
    }
}

void Fuzzy::cleanFuzzyRules(fuzzyRuleArray* aux){
    if(aux != NULL){
        // Esvaziando a memória alocada
        this->cleanFuzzyRules(aux->next);
        free(aux);
    }
}

const int motor_1_a   	= 3;
const int motor_1_b   	= 5;
const int motor_2_a		= 6;
const int motor_2_b		= 9;
const int sensor		= 2;
const int SENSOR_MAX	= 300;
const int SENSOR_MIN	= 50;
Fuzzy* fuzzy = new Fuzzy();


//---------------

float distancia()
{
  float tempo;
  
  // Emite sinal
  pinMode(sensor, OUTPUT);
  digitalWrite(sensor, HIGH);
  delayMicroseconds(100);
  digitalWrite(sensor, LOW);
  // Configura sensor para receber sinal
  pinMode(sensor, INPUT);
  tempo = pulseIn(sensor, HIGH);
  return tempo / 2 / 29.4;
}

void acelerar_motor_1(float velocidade)
{
  digitalWrite(motor_1_b, LOW);
  analogWrite(motor_1_a, velocidade);
}

void rear_motor_1(float velocidade)
{
  digitalWrite(motor_1_a, LOW);
  analogWrite(motor_1_b, velocidade);
}

void acelerar_motor_2(float velocidade)
{
  digitalWrite(motor_2_b, LOW);
  analogWrite(motor_2_a, velocidade);
}

void rear_motor_2(float velocidade)
{
  digitalWrite(motor_2_a, LOW);
  analogWrite(motor_2_b, velocidade);
}

void setup()
{
  Serial.begin(9600);
  
  pinMode(motor_1_a, OUTPUT);
  pinMode(motor_1_b, OUTPUT);
  pinMode(motor_2_a, OUTPUT);
  pinMode(motor_2_b, OUTPUT);
  
    ///Define input da distancia
    FuzzyInput* distancia = new FuzzyInput(1);
        ///Definicao do FuzzySet da distancia
    FuzzySet* bateu = new FuzzySet(0, 0, 10, 50);
    distancia->addFuzzySet(bateu);
    FuzzySet* pertao = new FuzzySet(30, 80, 80, 110);
    distancia->addFuzzySet(pertao);
    FuzzySet* perto = new FuzzySet(90, 130, 130, 160);
    distancia->addFuzzySet(perto);
    FuzzySet* media = new FuzzySet(150, 180, 180, 210);
    distancia->addFuzzySet(media);
    FuzzySet* longe = new FuzzySet(200, 230, 230, 250);
    distancia->addFuzzySet(longe);
    FuzzySet* seguro = new FuzzySet(240, 280, 300, 300);
    distancia->addFuzzySet(seguro);
        ///Adiciona distancia ao input
    fuzzy->addFuzzyInput(distancia);
    ///------------------------------------------


    ///Definicao de outputs
    FuzzyOutput* velocidade1 = new FuzzyOutput(1);
    FuzzyOutput* velocidade2 = new FuzzyOutput(2);
        ///FuzzySet de velocidade1 e velocidade2
    FuzzySet* rear1 = new FuzzySet(-255, -255, -200, 0);
    velocidade1->addFuzzySet(rear1);
    FuzzySet* baixo1 = new FuzzySet(0, 30, 30, 70);
    velocidade1->addFuzzySet(baixo1);
    FuzzySet* medio1 = new FuzzySet(60, 100, 100, 140);
    velocidade1->addFuzzySet(medio1);
    FuzzySet* alto1 = new FuzzySet(130, 180, 200, 200);
    velocidade1->addFuzzySet(alto1);
    FuzzySet* turbo1 = new FuzzySet(200, 230, 255, 255);
    velocidade1->addFuzzySet(turbo1);

    FuzzySet* rear2 = new FuzzySet(-255, -255, -200, 0);
    velocidade2->addFuzzySet(rear2);
    FuzzySet* baixo2 = new FuzzySet(0, 30, 30, 70);
    velocidade2->addFuzzySet(baixo2);
    FuzzySet* medio2 = new FuzzySet(60, 100, 100, 140);
    velocidade2->addFuzzySet(medio2);
    FuzzySet* alto2 = new FuzzySet(130, 180, 200, 200);
    velocidade2->addFuzzySet(alto2);
    FuzzySet* turbo2 = new FuzzySet(200, 230, 255, 255);
    velocidade2->addFuzzySet(turbo2);
        ///Adiciona velocidade1 ao output
    fuzzy->addFuzzyOutput(velocidade1);
    fuzzy->addFuzzyOutput(velocidade2);
    ///------------------------------------------

    ///Definindo Regras
        ///Definindo Antecedentes
    FuzzyRuleAntecedent* if_bateu = new FuzzyRuleAntecedent();
    if_bateu->joinSingle(bateu);
    FuzzyRuleAntecedent* if_pertao = new FuzzyRuleAntecedent();
    if_pertao->joinSingle(pertao);
    FuzzyRuleAntecedent* if_perto = new FuzzyRuleAntecedent();
    if_perto->joinSingle(perto);
    FuzzyRuleAntecedent* if_media = new FuzzyRuleAntecedent();
    if_media->joinSingle(media);
    FuzzyRuleAntecedent* if_longe = new FuzzyRuleAntecedent();
    if_longe->joinSingle(longe);
    FuzzyRuleAntecedent* if_seguro = new FuzzyRuleAntecedent();
    if_seguro->joinSingle(seguro);
        ///Definindo Consequentes
    FuzzyRuleConsequent* motor1_rear = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* motor2_rear = new FuzzyRuleConsequent();
    motor1_rear->addOutput(rear1);
    motor2_rear->addOutput(rear2);
    FuzzyRuleConsequent* motor1_baixo = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* motor2_baixo = new FuzzyRuleConsequent();
    motor1_baixo->addOutput(baixo1);
    motor2_baixo->addOutput(baixo2);
    FuzzyRuleConsequent* motor1_medio = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* motor2_medio = new FuzzyRuleConsequent();
    motor1_medio->addOutput(medio1);
    motor2_medio->addOutput(medio2);
    FuzzyRuleConsequent* motor1_alto = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* motor2_alto = new FuzzyRuleConsequent();
    motor1_alto->addOutput(alto1);
    motor2_alto->addOutput(alto2);
    FuzzyRuleConsequent* motor1_turbo = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* motor2_turbo = new FuzzyRuleConsequent();
    motor1_turbo->addOutput(turbo1);
    motor2_turbo->addOutput(turbo2);

        ///Definindo regras
    FuzzyRule* fuzzyRule01 = new FuzzyRule(1, if_bateu, motor1_rear);
    FuzzyRule* fuzzyRule02 = new FuzzyRule(2, if_bateu, motor2_rear);
    fuzzy->addFuzzyRule(fuzzyRule01);
    fuzzy->addFuzzyRule(fuzzyRule02);
    FuzzyRule* fuzzyRule03 = new FuzzyRule(3, if_pertao, motor1_medio);
    FuzzyRule* fuzzyRule04 = new FuzzyRule(4, if_pertao, motor2_baixo);
    fuzzy->addFuzzyRule(fuzzyRule03);
    fuzzy->addFuzzyRule(fuzzyRule04);
    FuzzyRule* fuzzyRule05 = new FuzzyRule(5, if_perto, motor1_medio);
    FuzzyRule* fuzzyRule06 = new FuzzyRule(6, if_perto, motor2_baixo);
    fuzzy->addFuzzyRule(fuzzyRule05);
    fuzzy->addFuzzyRule(fuzzyRule06);
    FuzzyRule* fuzzyRule07 = new FuzzyRule(7, if_media, motor1_baixo);
    FuzzyRule* fuzzyRule08 = new FuzzyRule(8, if_media, motor2_medio);
    fuzzy->addFuzzyRule(fuzzyRule07);
    fuzzy->addFuzzyRule(fuzzyRule08);
    FuzzyRule* fuzzyRule09 = new FuzzyRule(9, if_longe, motor1_alto);
    FuzzyRule* fuzzyRule10 = new FuzzyRule(10, if_longe, motor2_alto);
    fuzzy->addFuzzyRule(fuzzyRule09);
    fuzzy->addFuzzyRule(fuzzyRule10);
    FuzzyRule* fuzzyRule11 = new FuzzyRule(11, if_seguro, motor1_turbo);
    FuzzyRule* fuzzyRule12 = new FuzzyRule(12, if_seguro, motor2_turbo);
    fuzzy->addFuzzyRule(fuzzyRule11);
    fuzzy->addFuzzyRule(fuzzyRule12);
    ///------------------------------------------------------------  
  
}

void loop()
{
  float dist = distancia();
  fuzzy->setInput(1, dist);
  fuzzy->fuzzify();
  float v1 = fuzzy->defuzzify(1);
  float v2 = fuzzy->defuzzify(2);
  if (v1 > 0) acelerar_motor_1(v1);
  else rear_motor_1(v1 * (-1));
  if (v2 > 0) acelerar_motor_2(v2);
  else rear_motor_2(v2 * (-1));
}