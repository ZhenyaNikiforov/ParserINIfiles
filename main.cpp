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
      cout << "File \"" << this->pathToFile << "\" unknow." << endl; //...сообщаем об этом
      iniFile.close();                                               // закрываем файл,
      return "error";                                                // прерываем подпрограмму
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

      if (regex_match(lineFromFile, patternSectionName)) // если имя секции
      {
        treeSets.push_back({lineFromFile});
      };

      if (regex_match(lineFromFile, patternTypeLine)) // если тип строки
      {
        treeSets[treeSets.size() - 1].push_back(lineFromFile);
      };
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
  ini_parser parser("./config.ini");
  string value = parser.getValue("[Section2].var10");

  cout << "znachenie: " << value << endl;

  return 0;
}