/*-Парсер .INI-файлов-*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
using namespace std;

class ini_parser // класс нашего парсера
{
public:
  ini_parser(string fileName) // конструктор, принимает имя (и путь) к файлу
  {
    this->pathToFile = fileName; // заносим имя и путь к файлу в поле экземпляра;
  };

  void getValue(string adressLine) // подпрограмма взятия значения переменной
  {
    ifstream iniFile(this->pathToFile); // открываем ini-файл
    if (iniFile.is_open() == false)     // если не открылся...
    {
      cout << "File \"" << this->pathToFile << "\" unknow." << endl; //...сообщаем об этом
      iniFile.close();                                               // закрываем файл,
      return;                                                        // прерываем подпрограмму
    };

    cout << endl;
    cout << "Otkr.!, (fajl otkryilsya!)" << endl; // потом убрать! Не забыть!

    string sectionName = "";                    // название секции
    string variableName = "";                   // название переменной
    bool pointTerminator = false;               // точки нет пока
    for (int i = 0; i < adressLine.size(); ++i) // парсим строку адреса
    {
      if ((pointTerminator == false) && (adressLine[i] != '.'))
      {
        sectionName.push_back(adressLine[i]);
        continue;
      };
      if ((pointTerminator == false) && (adressLine[i] == '.'))
      {
        pointTerminator = true;
        continue;
      };
      if ((pointTerminator == true) && (adressLine[i] != '.'))
      {
        variableName.push_back(adressLine[i]);
        continue;
      }
    };

    cout << "Nazvanie sekcii: " << sectionName << endl;  // потом убрать
    cout << "Imya peremennoj: " << variableName << endl; // тоже
    cout << endl;

    string lineFromFile = "";                                      // текущая строка из файла
    regex patternSectionName("\\[[a-zA-Z0-9_]+\\]");               // шаблон имени секции
    regex patternTypeLine("[\\s\\S]+=[\\s\\S]*");                  // шаблон строки, содержащей знак "="
    vector<vector<string>> treeSets = {};                          // дерево наборов
    regex patternOurVariable(variableName + "\\s*=\\s*[\\s\\S]*"); // шаблон строки с искомой переменной

    while (true) // парсим файл
    {
      if (iniFile.eof()) // если файл кончился, то...
      {
        iniFile.close(); //...закрываем его
        break;           //...прерываем цикл
      };
      lineFromFile.clear();                                  // очищаем текущую строку
      getline(iniFile, lineFromFile);                        // читаем строку из файла
      this->cleansUpSpacesInBeginningAndInEnd(lineFromFile); // пробелы в начале и конце

      cout << lineFromFile << endl; // выводим на экран

      if (regex_match(lineFromFile, patternSectionName)) //
      {
        treeSets.push_back({lineFromFile}); //
      };

      if (regex_match(lineFromFile, patternTypeLine)) //
      {
        treeSets[treeSets.size() - 1].push_back(lineFromFile); //
      };
      //-Это только для отладки
      if (regex_match(lineFromFile, patternOurVariable))
      {
        cout << "EST` VAR1!!!" << endl;
      };
    };

    cout << endl;
    cout << "Pokazyivayem strukturu grafa:" << endl;
    cout << endl;

    for (int Set = 0; Set < treeSets.size(); ++Set)
    {
      for (int Line = 0; Line < treeSets[Set].size(); Line++)
      {
        cout << treeSets[Set][Line] << endl;
      };
      cout << endl;
    };

    cout << "Perebiraem derevo spiskov:" << endl;
    cout << endl;
    string endResult = ""; // строка окончательного результата

    for (int Set = 0; Set < treeSets.size(); Set++)
    {
      if (treeSets[Set][0] == sectionName)
      {
        cout << "Sekciya, kotoryu myi ishem: ";
        cout << treeSets[Set][0] << endl;
        for (int Line = 1; Line < treeSets[Set].size(); Line++)
        {
          if (regex_match(treeSets[Set][Line], patternOurVariable))
          {
            cout << "Stroka, kotoruy myi ishem: ";
            cout << treeSets[Set][Line] << endl;

            char paritySignum = '=';                                                // знак "равно"
            size_t positionParitySignum = treeSets[Set][Line].find(paritySignum);   // место знака "равно"
            string mezoLine = treeSets[Set][Line].substr(positionParitySignum + 1); // строка после знака "равно"
            cout << "stroka posle znaka = :" << mezoLine << endl;

            char commentSignum = ';'; // символ комментария
            string finishedLine = "";
            size_t positionCommentSignum = mezoLine.find(commentSignum); // место комментария                                                                // финишн. строка
            if (positionCommentSignum != string::npos)
            {
              finishedLine = mezoLine.substr(0, positionCommentSignum);
            }
            else
            {
              finishedLine = mezoLine;
            };
            cout << "finishnaya stroka :" << finishedLine << endl;
            cleansUpSpacesInBeginningAndInEnd(finishedLine);
            cout << "finish. str. bez probelov :" << finishedLine << endl;

            endResult.clear();
            endResult = finishedLine;
          }
        }
      }
    }

    cout << "Konechnyij rezultat f-csii :" << endResult << endl;

    return; // прерываем подпрограмму
  };

  void cleansUpSpacesInBeginningAndInEnd(string &line)
  {
    line.erase(0, line.find_first_not_of(" \t\r\n\v\f")); // пробелы в начале
    line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1); // пробелы в конце
    return;
  }

private:
  string pathToFile = "";
};

int main()
{
  ini_parser parser("./config.ini");
  parser.getValue("[Section3].Mode");
  /*int value = parser.get_value("[Section2].var1");//<int>

  cout << "znachenie: " << value << endl;*/
  return 0;
}