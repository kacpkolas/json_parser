//g++ (Rev6, Built by MSYS2 project) 12.2.0
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;
void help();
bool bracket_check(char*& buffer_text, char*& brackets, int j, int& size, int end_of_line, int readings, int offset, char bracketopen, char bracketclose, bool& end); //?


const int READING_SIZE = 100;  // ile bajtów powinien jednorazowo czytać program z pliku json
const int STRING_LIMIT = 200; // limit na input

int main()
{
    char choice[STRING_LIMIT];
    while (true)
    {
        for (int i = 0; i < STRING_LIMIT; i++)
            choice[i] = 0;

        // wybór opcji
        cout << "wybor pliku / pomoc:" << endl;

        fgets(choice, STRING_LIMIT, stdin);

        if (choice[0] == '\n' && strlen(choice) > 1)
            for (int i = 0; i < STRING_LIMIT - 1; i++)
            {
                choice[i] = choice[i + 1];
                fgets(choice, STRING_LIMIT, stdin);
            }
        if (choice[0] == '\n')
            choice[0] = 0;
        choice[strlen(choice) - 1] = 0; // usuwanie /n
        switch (choice[0])
        {
            // help
        case 'h':
        {
            help();
            break;
        }
        // otwarcie pliku o nazwie nazwa_pliku
        case 'o':
        {
            if (choice[1] != ' ')
            {
                cout << "Niepoprawna opcja" << endl;
                break;
            }
            // otwieram plik
            fstream file;
            char path[STRING_LIMIT - 2];
            for (int i = 0; i < STRING_LIMIT - 2; i++)
                path[i] = choice[i + 2];  

            file.open(path);
            // sprawdzam czy się poprawnie otworzył
            if (file.is_open() == true && path[strlen(path) - 1] == 'n' && path[strlen(path) - 2] == 'o' && path[strlen(path) - 3] == 's' && path[strlen(path) - 4] == 'j' && path[strlen(path) - 5] == '.')
                cout << "Otworzono poprawnie plik" << endl;
            else
            {
                cout << "Nieudana proba otwarcia pliku badz plik nie jest typu JSON" << endl;
                break;
            }

            cout << "Wybierz opcje co zrobic z plikiem:" << endl;
            for (int i = 0; i < STRING_LIMIT; i++)
                choice[i] = 0;

            fgets(choice, STRING_LIMIT, stdin); //


            choice[strlen(choice) - 1] = 0; // usuwanie /n
            if (choice[1] != 0)
                choice[0] = 0;

            switch (choice[0])
            {
                // operacja sprawdza czy plik jest poprawnym formatem JSON
            case 'c':
            {
                int size = 1;           //rozmiar tab dynamincznej
                char* brackets = new char[size]; //
                brackets[0] = 0; //1 eleemtn
                bool text = false; //otwarcie cudzyslowia
                bool comma = false; //przecinek lub dwukropek(czy nie obok siebie)
                int readings = -1;  //
                int end_of_line = 0; //numer w pionie 
                int offset = -1; //przesuniecie w poziomie
                bool firstbracket = false; //pierwszy nawias
                bool firstdata = false; //pierwsze dane
                bool no_error = true; //error
                bool end_of_file = false; //koniec pliku 
                bool data = false;  //dane po przecinku 

                while (file && no_error)
                {
                    //zerowanie tablicy
                    char* buffer_text = new char[READING_SIZE];
                    for (int i = 0; i < READING_SIZE; i++)
                        buffer_text[i] = ' ';

                    // odczytaj zadaną ilość bajtów
                    file.read(buffer_text, READING_SIZE);
                    readings++;
                    // odczytaj ilość odczytanych bajtów
                    int count = file.gcount();
                    // jeżeli nic nie odczytano przerwij
                    if (!count)
                    {
                        file.close();
                        break;
                    }

                    for (int i = 0; i < READING_SIZE; i++)
                    {

                        if (i > count - 1)
                        {
                            file.close();
                            break;
                        }

                        if (buffer_text[i] == 10)
                        {
                            end_of_line++;
                            readings = 0;
                            offset = i;
                        }
                        // sprawdzenie czy wystąpił nawias oraz czy json zaczyna się tak jak powinien
                        if (buffer_text[i] == '{' || buffer_text[i] == '[' || buffer_text[i] == '(')
                            firstbracket = true;

                        if (buffer_text[i] != '\n' && buffer_text[i] != ' ' && buffer_text[i] != '\t' && firstbracket == false)
                        {  
                            cout << "brak rozpoczecia root:" << end_of_line + 1 << ":" << readings * READING_SIZE + i - offset << endl; //?
                            no_error = false;
                            file.close();
                            break;
                        }

                        if (end_of_file == true && brackets[i] != ' ' && brackets[i] != '\t' && brackets[i] != '\n' && brackets[i] != 0) // sprawdzam czy są dane po zakończeniu głównego nawiasu
                        {
                            char wtf = brackets[i];
                            cout << "Dane po zakonczeniu ROOT:" << end_of_line + 1 << ":" << readings * READING_SIZE + i - offset << endl;  //?
                            no_error = false;
                            file.close();
                            break;
                        }

                        // sprawdzanie poprawności nawiasów i cudzyslowiow
                        if (bracket_check(buffer_text, brackets, i, size, end_of_line, readings, offset, '[', ']', end_of_file))
                        {
                            no_error = false;
                            file.close();
                            break;
                        }
                        else if (bracket_check(buffer_text, brackets, i, size, end_of_line, readings, offset, '(', ')', end_of_file))
                        {
                            no_error = false;
                            file.close();
                            break;
                        }
                        else if (bracket_check(buffer_text, brackets, i, size, end_of_line, readings, offset, '{', '}', end_of_file))
                        {
                            no_error = false;
                            file.close();
                            break;
                        }

                        // zapis że wystąpiło wpisanie jakichkolwiek danych po pierwszym nawiasie
                        if (buffer_text[i] != ':' && buffer_text[i] != ',' && buffer_text[i] != 10 && buffer_text[i] != '{' && buffer_text[i] != '[' && buffer_text[i] != ' ' && buffer_text[i] != '\t')
                            firstdata = true;

                        if (buffer_text[i] != ':' && buffer_text[i] != ',' && buffer_text[i] != 10 && buffer_text[i] != '{' && buffer_text[i] != '[' && buffer_text[i] != ' ' && buffer_text[i] != '\t')
                            data = true;
                        if (buffer_text[i] != ',')
                            data = false;

                        if (data == true && (buffer_text[i] == '{' || buffer_text[i] == '['))
                        {
                            cout << "brak przecinka:" << end_of_line + 1 << ":" << readings * READING_SIZE + i - offset << endl;
                            no_error = false;
                            file.close();
                            break;
                        }

                        // Błąd z wyniku postawienia dwukropka lub przecinku zanim podano jakiekolwiek dane
                        if (firstbracket == false || firstdata == false)
                            if (buffer_text[i] == ':' || buffer_text[i] == ',')
                            {
                                cout << "przecinek lub dwukropek przed danymi:" << end_of_line + 1 << ":" << readings * READING_SIZE + i - offset << endl;
                                no_error = false;
                                file.close();
                                break;
                            }

                        // załączanie pola tekstowego
                        if (buffer_text[i] == '"')
                            text = !text;

                        // dane literowe bez cudzysłowiu
                        if (((buffer_text[i] >= 65 && buffer_text[i] <= 90) || (buffer_text[i] >= 97 && buffer_text[i] <= 122)) && text == false)
                        {
                            cout << "dane literowe bez cudzyslowiu:" << end_of_line + 1 << ":" << readings * READING_SIZE + i - offset << endl;
                            no_error = false;
                            file.close();
                            break;
                        }

                        // pomijanie symboli jako błędu
                        if (text == true && buffer_text[i] == '\\')
                            i++;
                        else if (text == false && buffer_text[i] == '\\')
                        {
                            cout << "symbol \"\\\" poza tekstem " << end_of_line + 1 << ":" << readings * READING_SIZE + i - offset << endl;
                            no_error = false;
                            file.close();
                            break;
                        }


                        // sprawdzanie podwójnego dwukropka/przecinka
                        if ((buffer_text[i] == ',' || buffer_text[i] == ':') && text == false && comma == false)
                            comma = true;

                        else if (comma == true && buffer_text[i] != ' ' && buffer_text[i] != ':' && buffer_text[i] != ',')
                            comma = false;

                        else if (text == false && (buffer_text[i] == ':' || buffer_text[i] == ',') && comma == true)
                        {
                            cout << "blad podojny dwukropek/przecinek poza tekstem" << end_of_line + 1 << ":" << readings * READING_SIZE + i - offset << endl;
                            no_error = false;
                            file.close();
                            break;
                        }
                    }
                }
                if (brackets[0] < 0)
                    brackets[0] = 0;
                if (brackets[0] != 0 && no_error == true && brackets[0] != '0') 
                    cout << "Plik niepoprawny brakuje zamkniecia nawiasu" << endl;
                else if (text != false && no_error == true)
                    cout << "Plik niepoprawny brakuje cudzyslowiu" << endl;
                else if (no_error == true)
                    cout << "Plik poprawny" << endl;
                delete[] brackets;
                file.close();
                break;
            }
            default:
            {
                cout << "Wybrano zla opcje, powrot do wyboru pliku" << endl;
                file.close();
                break;
            }
            }
            break;
        }
        default:
        {
            cout << "Niepoprawna opcja" << endl;
            break;
        }
        }
    }
    return 0;
}

// funkcja drukująca instrukcje.
void help()
{
    cout << R"(
Autor: Kacper Kolassa 193654

Instrukcja:
h - wyswietlanie pomocoy
o nazwa_pliku - otwarcie pliku o nazwie nazwa_pliku
c - operacja sprawdza czy plik jest poprawnym formatem JSON,
)" << '\n';
}

bool bracket_check(char*& buffer_text, char*& brackets, int j, int& size, int end_of_line, int readings, int offset, char bracketopen, char bracketclose, bool& end)
{//sprawdzenie czy wystapil nawias
    if (buffer_text[j] == bracketopen)
    {       //tablica o rozm tab brackets
        char* buffer_array = new char[size];
        for (int i = 0; i < size; i++)
            buffer_array[i] = brackets[i];

        delete[] brackets;
        brackets = buffer_array;
        buffer_array = NULL;
        brackets[size - 1] = bracketopen;
        size++; //tworzenie nowej tablicy i kopiowanie starej i nadawanie jej tej samej nazwy i powiekszanie
    }
    else if (buffer_text[j] == bracketclose)
        if (brackets[size - 2] == bracketopen)
        {
            if (size - 2 == 0) {
                end = true;
                brackets[0] = 0;
            
            }
            else {
                size--;
                char* buffer_array = new char[size - 1];
                for (int i = 0; i < size - 1; i++)
                    buffer_array[i] = brackets[i];

                delete[] brackets;
                brackets = buffer_array;
                buffer_array = NULL;//tworzenie mniejszej tablicy i kopiowanie o element mniej 

            }
            
        }
        else
        {
            cout << "Blad nieprawidlowy nawias " << end_of_line + 1 << ":" << readings * READING_SIZE + j - offset << endl;
            return true;
        }
    return false;
}
