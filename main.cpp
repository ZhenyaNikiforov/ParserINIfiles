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

  string getValue(string adressLine) // подпрограмма взятия значения переменной
  {
    ifstream iniFile(this->pathToFile); // открываем ini-файл
    if (iniFile.is_open() == false)     // если не открылся...
    {
      // cout << "File \"" << this->pathToFile << "\" unknow." << endl; //...сообщаем об этом
      iniFile.close(); // закрываем файл,
      throw invalid_argument("ERROR! File \"" + this->pathToFile + "\" ne najden.");
      return "error"; // прерываем подпрограмму
    };

    string sectionName = "";  // название секции
    string variableName = ""; // название переменной

    size_t positionPointSeparator = adressLine.find('.');         // номер точки в строке
    sectionName = adressLine.substr(0, positionPointSeparator);   // имя секции - первая часть
    variableName = adressLine.substr(positionPointSeparator + 1); // имя переменной - вторая часть

    string lineFromFile = "";                                      // текущая строка из файла
    regex patternSectionName("\\[[a-zA-Z0-9_]+\\]");               // шаблон имени секции
    regex patternTypeLine("[\\s\\S]+=[\\s\\S]*");                  // шаблон строки, содержащей знак "="
    vector<vector<string>> treeSets = {};                          // дерево наборов
    regex patternOurVariable(variableName + "\\s*=\\s*[\\s\\S]*"); // шаблон строки с искомой переменной
    regex patternErrorLine("[\\s\\S]+=\\s*");                      // шаблон ошибки строки
    int lineCount = 1;                                             // подсчёт строк

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

      if (regex_match(lineFromFile, patternErrorLine))
      {
        throw invalid_argument("ERROR! At line " + to_string(lineCount) + ": " + lineFromFile);
      };

      if (regex_match(lineFromFile, patternSectionName)) // если имя секции
      {
        treeSets.push_back({lineFromFile});
      };

      if (regex_match(lineFromFile, patternTypeLine)) // если тип строки
      {
        treeSets[treeSets.size() - 1].push_back(lineFromFile);
      };
      lineCount++;
    };

    string endResult = ""; // строка окончательного результата

    for (int Set = 0; Set < treeSets.size(); Set++)
    {
      if (treeSets[Set][0] == sectionName)
      {
        for (int Line = 1; Line < treeSets[Set].size(); Line++)
        {
          if (regex_match(treeSets[Set][Line], patternOurVariable))
          {
            size_t positionParitySignum = treeSets[Set][Line].find('=');            // место знака "равно"
            string mezoLine = treeSets[Set][Line].substr(positionParitySignum + 1); // строка после знака "равно"

            string finishedLine = "";
            size_t positionCommentSignum = mezoLine.find(';'); // место комментария                                                                // финишн. строка
            if (positionCommentSignum != string::npos)
            {
              finishedLine = mezoLine.substr(0, positionCommentSignum);
            }
            else
            {
              finishedLine = mezoLine;
            };
            cleansUpSpacesInBeginningAndInEnd(finishedLine);
            endResult.clear();
            endResult = finishedLine;
          }
        }
      }
    }
    if (endResult == "")
    {
      throw invalid_argument("ERROR! Peremennaya \"" + variableName + "\" ne najdena v \"" + sectionName + "\"");
    }
    return endResult; // прерываем подпрограмму
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
  cout << endl;
  ini_parser parser("./config.ini");

  try
  {
    cout << parser.getValue("[Section1].var1") << endl;
    cout << parser.getValue("[Section1].var2") << endl;
    cout << parser.getValue("[Section1].var10") << endl;
  }
  catch (const invalid_argument &e)
  {
    cout << e.what() << endl; // ошибка ненайденной переменной
  }

  cout << endl;
  ini_parser parser2("./configg.ini");

  try
  {
    cout << parser2.getValue("[Section1].var1") << endl;
  }
  catch (const invalid_argument &e)
  {
    cout << e.what() << endl; // ошибка непрочитанного файла
  }

  cout << endl;
  ini_parser parser3("./configErr.ini");

  try
  {
    cout << parser3.getValue("[Section2].var2") << endl;
  }
  catch (const invalid_argument &e)
  {
    cout << e.what() << endl; //
  }
  return 0;
}