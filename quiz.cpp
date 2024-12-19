#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random> // shuffle domande 
#include <algorithm>
#include <iomanip>
#include <chrono> 
#include "welcome.h"

using namespace std;

// struttura domande
struct Question {
    string question;
    vector<string> options; // opzioni a - b - c - d ed anche e se è una domanda speciale
    int correctAnswer; // posizione della risposta corretta 
    bool isSpecial; // se è una domanda speciale
};

// calcolo rank letterale da SS a F basato sulla percentuale (risposte giuste / domande a cui si ha risposto).
string calculateRank(float percentage) {
    if (percentage == 100) return "SS";
    else if (percentage >= 95) return "S";
    else if (percentage >= 90) return "A";
    else if (percentage >= 80) return "B";
    else if (percentage >= 70) return "C";
    else if (percentage >= 60) return "D";
    else if (percentage >= 50) return "E";
    else return "F";
}

// restituisce TRUE o FALSE se la domanda contiene un spoiler o meno
bool containsSpoiler(const string& question, string& gameName) {
    size_t posStart = question.find("[spoiler (");
    size_t posEnd = question.find(")]");

    if (posStart != string::npos && posEnd != string::npos && posEnd > posStart) {
        gameName = question.substr(posStart + 10, posEnd - (posStart + 10));
        return true;
    }
    return false;
}

// lettura domande dai file questions.txt e special.txt
vector<Question> readQuestions(const string& fileName, bool isSpecial = false) {
    vector<Question> questions;
    ifstream file(fileName);

    if (!file) {
        cerr << "Non sono riuscito ad aprire il file " << fileName << endl;
        exit(1);
    }
    
    string line;
    Question q;
    bool readingQuestion = false;
    
    // ciclo di lettura domande. 
    // Q, simboleggia inizio domanda mentre E, la fine.
    while (getline(file, line)) {
        if (line.substr(0, 2) == "Q,") {
            readingQuestion = true;
            q = Question();
            q.isSpecial = isSpecial;
            q.question = line.substr(2);
        } else if (line.substr(0, 2) == "E,") {
            readingQuestion = false;
            questions.push_back(q);
            // la risposta giusta è flaggata con !
        } else if (readingQuestion && !line.empty()) {
            if (line[0] == '!') {
                q.correctAnswer = q.options.size();
                line = line.substr(1);
            }
            q.options.push_back(line);
        }
    }

    return questions;
}

// selettore difficoltà
string getDifficulty() {
    while (true) {
        cout << "Seleziona difficoltà\n";
        cout << "E - Easy (99 vite)\n";
        cout << "N - Normal (3 vite)\n";
        cout << "H - Hard (2 vite)\n";
        cout << "X - Extreme (1 vita)\n";
        cout << "Scelta: ";

        char choice;
        cin >> choice;
        choice = tolower(choice);

        switch (choice) {
            case 'e': return "easy";
            case 'n': return "normal";
            case 'h': return "hard";
            case 'x': return "extreme";
            default:
                cout << "La scelta non è valida, riprova\n\n";
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
        }
    }
}

// domande a cui si è risposto richieste per passare il test. 
int getRequiredQuestions(const string& difficulty) {
    if (difficulty == "easy") return 12;
    else if (difficulty == "normal") return 15;
    else if (difficulty == "hard") return 18;
    else if (difficulty == "extreme") return 25;
}

// vite a seconda della difficoltà. Se scendono a 0, il test viene considerato FALLITO.
int getLives(const string& difficulty) {
    if (difficulty == "easy") return 99;
    else if (difficulty == "normal") return 3;
    else if (difficulty == "hard") return 2;
    else if (difficulty == "extreme") return 1;
    else return 3;
}


// chiede la risposta all'utente. Se la risposta non è valida, la chiede di nuovo. Se la domanda è speciale , ci saranno 5 risposte possibili anzichè 4.
char getValidAnswer(bool isSpecial) {
    while (true) {
        char maxOption = isSpecial ? 'E' : 'D';
        cout << "La tua risposta (A-" << maxOption << "): ";
        char answer;
        cin >> answer;
        answer = toupper(answer);

        switch (answer) {
            case 'A': return 'A';
            case 'B': return 'B';
            case 'C': return 'C';
            case 'D': return 'D';
            case 'E':
                if (isSpecial) return 'E';
            default:
                cout << "Input non valido , riprova. \n";
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
        }
    }
}

// converte la risposta dell'utente in un indice.
int convertAnswerToIndex(char answer) {
    switch (answer) {
        case 'A': return 0;
        case 'B': return 1;
        case 'C': return 2;
        case 'D': return 3;
        case 'E': return 4;
        default: return -1;
    }
}

int main() {
    displayWelcome(); // welcome screen
    // lettura domande
    vector<Question> questions = readQuestions("questions.txt");
    vector<Question> specialQuestions = readQuestions("special.txt", true);
    // lettura difficoltà
    string difficulty = getDifficulty();
    int lives = getLives(difficulty);
    int requiredQuestions = getRequiredQuestions(difficulty);

    // mescolaggio domande piu' avanzato. 

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> specialDist(1, 5);

    auto timeNow = chrono::system_clock::now().time_since_epoch().count();
    shuffle(questions.begin(), questions.end(), mt19937(rd() + timeNow));
    shuffle(specialQuestions.begin(), specialQuestions.end(), mt19937(rd() + timeNow + 1));

    // stato variabili all'inizio del test
    int score = 0;
    int attemptedQuestions = 0;
    
    // ciclo di domande
    while (attemptedQuestions < requiredQuestions && lives > 0) {
        Question currentQ;
        if (specialDist(gen) == 1 && !specialQuestions.empty()) {
            currentQ = specialQuestions.front();
            specialQuestions.erase(specialQuestions.begin());
        } else if (!questions.empty()) {
            currentQ = questions.front();
            questions.erase(questions.begin());
        } else {
            break;
        }
        
        // segnalibro domanda.
        cout << "\nQuestion " << attemptedQuestions + 1 << ":\n";
        
        // se la domanda contiene un spoiler, chiede all'utente se vuole rispondere. Se rifiuta , ne verrà proposta un altra.
        string gameName;
        if (containsSpoiler(currentQ.question, gameName)) {
            cout << "Attenzione, questa domanda contiene un spoiler \"" << gameName << "\". Vuoi rispondere? (y/n): ";
            char choice;
            cin >> choice;
            if (tolower(choice) != 'y') {
                continue;
            }
        }
        
        // stampa domanda
        cout << currentQ.question << endl;
        for (size_t i = 0; i < currentQ.options.size(); i++) {
            cout << char('A' + i) << ") " << currentQ.options[i] << endl;
        }
        
        // qualsiasi sia la risposta , incrementa il contatore delle domande risposte di uno.
        char userAnswer = getValidAnswer(currentQ.isSpecial);
        int answerIndex = convertAnswerToIndex(userAnswer);
        attemptedQuestions++;
        
        // controlla se la risposta è corretta. Se è corretta il punteggio aumenta di uno, se è sbagliata la vita decrementa di uno. In difficoltà estrema fallire una domanda decreta il GAME OVER istantaneo.
        if (answerIndex == currentQ.correctAnswer) {
            cout << "Risposta giusta! era proprio " << char('A' + currentQ.correctAnswer) << "\n";
            score++;
        } else {
            cout << "Sbagliata! La risposta giusta era " << char('A' + currentQ.correctAnswer) << endl;
            lives--;
        }
        
        // restituisce all'utente quante vite ha.
        if (lives > 1) {
            cout << "Vite rimaste: " << lives << endl;
        }
        if (lives == 1) {
            cout << "Attenzione, ti è rimasta solo 1 vita, se ne sbagli un altra fallirai! dacci dentro!" << endl;
        }
    }
    

    // scenario di game over
    if (lives <= 0) {
        cout << "\nGame Over! Hai terminato le vite" << endl;
        cout << "Hai completato " << attemptedQuestions << " risposte su " << requiredQuestions << " richieste per passare il test" << endl;
    // improbabile scenario quando le domande sono esaurite.
    } else if (questions.empty() && specialQuestions.empty()) {
        cout << "\nNon ci sono piu' domande disponibili!" << endl;
    }
    
    // calcolo percentuale tra domande a cui si è data la risposta corretta e domande a cui si ha effettivamente risposto.
    float percentage = (score / attemptedQuestions) * 100;
    string rank = calculateRank(percentage);

    // stampa risulutato.
    cout << "\nRisultati finali\n";
    cout << "Domande a cui hai risposto: " << attemptedQuestions << "/" << requiredQuestions << endl;
    cout << "Domande giuste " << score << endl;
    cout << "Percentuale di risposte giuste " << fixed << setprecision(2) << percentage << "%" << endl;
    cout << "Rank: " << rank << endl;

    return 0;
}
