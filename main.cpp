#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>

using namespace std;

vector<string> readFile(const string& fileName) {
    vector<string> content;
    ifstream file(fileName);
    if (!file) {
        cerr << "Non sono riuscito ad aprire il file: " << fileName << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        content.push_back(line);
    }
    file.close();
    return content;
}

bool containsSpoiler(const string& question, string& gameName) {
    size_t posStart = question.find("[spoiler (");
    size_t posEnd = question.find(")]");

    if (posStart != string::npos && posEnd != string::npos && posEnd > posStart) {
        gameName = question.substr(posStart + 10, posEnd - (posStart + 10));
        return true;
    }
    return false;
}

string calculateRank(float percentage) {
    if (percentage == 100) return "SS";
    else if (percentage >= 95) return "S";
    else if (percentage >= 90) return "A";
    else if (percentage >= 80) return "B";
    else if (percentage >= 70) return "C";
    else if (percentage >= 60) return "D";
    else if (percentage >= 50) return "E";
    else return "G";
}

int main() {
    cout << "Benvenuto al quiz! Preparati a rispondere a delle domande. Vediamo quanto riesci a fare!" << endl;

    vector<string> questions = readFile("questions.txt");
    vector<string> answers = readFile("answers.txt");

    int totalQuestions = questions.size() / 6; 
    if (answers.size() != totalQuestions) {
        cerr << "il numero di risposte non corrisponde alle domande!" << endl;
        return 1;
    }

    int score = 0;
    int attemptedQuestions = 0;

    for (int i = 0, questionIndex = 0; i < questions.size(); i += 6, ++questionIndex) {
        cout << endl;

        string gameName;
        if (containsSpoiler(questions[i], gameName)) {
            cout << "Attenzione! Questa domanda potrebbe contenere uno spoiler su \"" << gameName << "\". Vuoi rispondere? (s/n): ";
            char choice;
            cin >> choice;
            if (tolower(choice) == 'n') {
                cout << "Hai scelto di saltare questa domanda. Non sarà conteggiata." << endl;
                continue;
            }
        }

        for (int j = 0; j < 5; ++j) {
            cout << questions[i + j] << endl;
        }

        char userAnswer;
        cout << "Inserisci la tua risposta (a/b/c/d): ";
        cin >> userAnswer;

        attemptedQuestions++;

        if (tolower(userAnswer) == answers[questionIndex][0]) {
            cout << "Bravo! La tua risposta è corretta." << endl;
            score++;
        } else {
            cout << "Peccato! La risposta corretta era: " << answers[questionIndex] << endl;
        }
    }

    cout << endl;
    cout << "Hai completato il quiz!" << endl;

    if (attemptedQuestions > 0) {
        float percentage = (static_cast<float>(score) / attemptedQuestions) * 100;
        string rank = calculateRank(percentage);
        cout << "Hai risposto correttamente a " << score << " domande su " << attemptedQuestions << "." << endl;
        cout << "La tua percentuale di successo è " << percentage << "%." << endl;
        cout << "Il tuo rank è: " << rank << "." << endl;
    } else {
        cout << "Non hai risposto a nessuna domanda, quindi non hai un rank." << endl;
    }

    cout << "Grazie per aver partecipato!" << endl;

    return 0;
}
