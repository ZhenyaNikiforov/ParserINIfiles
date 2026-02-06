/*-Парсер .INI-файлов-*/

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <regex>
#include <type_traits>
using namespace std;

class ini_parser // класс нашего парсера
{
public:
  ini_parser(string pathAndNameFile) // конструктор, принимает имя (и путь) к файлу
  {
    ifstream iniFile(pathAndNameFile); // открываем ini-файл

    if (iniFile.is_open() == false) // если не открылся...
    {
      iniFile.close();                                                           // закрываем файл
      throw invalid_argument("Oshibka! \"" + pathAndNameFile + "\" ne najden!"); // выводим ошибку (сообщаем)
    };

    string lineFromFile = "";                                          // строка из файла
    regex patternSectionName("\\s*\\[Section[0-9]+\\]\\s*");           // имя секции
    regex patternInfoLine("\\s*[a-zA-Z]\\w*\\s*=[^;]*[\\w][\\s\\S]*"); // инф. строка
    regex patternCommentLine("\\s*;[\\s\\S]*");                        // строка коммент.
    regex patternEmptyLine("\\s*");                                    // пустая строка
    string currentSectionName = "";                                    // текущ. секц.
    int countLine = 1;                                                 // подсчёт строк

    while (true) // парсим файл
    {
      if (iniFile.eof()) // если файл кончился, то...
      {
        iniFile.close(); //...закрываем его
        break;           //...прерываем цикл
      };

      lineFromFile.clear();           // очищаем строку
      getline(iniFile, lineFromFile); // читаем строку из файла

      bool itIsSection = regex_match(lineFromFile, patternSectionName);
      bool itIsInfo = regex_match(lineFromFile, patternInfoLine);
      bool itIsComment = regex_match(lineFromFile, patternCommentLine);
      bool itIsEmpty = regex_match(lineFromFile, patternEmptyLine);

      if (itIsSection)
      {
        currentSectionName.clear();
        currentSectionName = lineFromFile;
        this->cleansUpSpacesInBeginningAndInEnd(currentSectionName);
        currentSectionName.pop_back();
        currentSectionName.erase(0, 1);
        this->treeMembers[currentSectionName][""] = "";
        countLine++;
        continue;
      };

      if (itIsInfo)
      {
        string variableName = lineFromFile.substr(0, lineFromFile.find('='));
        this->cleansUpSpacesInBeginningAndInEnd(variableName);
        string valueAndComment = lineFromFile.substr(lineFromFile.find('=') + 1);
        string variableValue = "";
        if (valueAndComment.find(';') != string::npos)
        {
          variableValue = valueAndComment.substr(0, valueAndComment.find(';'));
        }
        else
        {
          variableValue = valueAndComment;
        };
        this->cleansUpSpacesInBeginningAndInEnd(variableValue);
        this->treeMembers[currentSectionName][variableName] = variableValue;
        countLine++;
        continue;
      };

      if (!itIsSection && !itIsInfo && !itIsComment && !itIsEmpty)
      {
        throw invalid_argument("ERROR at line " + to_string(countLine) + ": \"" + lineFromFile + "\"");
        countLine++; // глупость
        continue;    // ещё большая
      };
      countLine++;
    };
  };

  template <typename T>
  T getValue(string section, string variable) // подпрограмма взятия значения переменной
  {
    T universalResult;

    if (this->treeMembers.find(section) == this->treeMembers.end())
    {
      cout << "Net sekcii \"" << section << "\", a est:" << endl;

      for (auto const &[sect, map] : this->treeMembers)
      {
        cout << sect << endl;
      };

      throw invalid_argument("Ne najdena sekciya \"" + section + "\"");
      return universalResult;
    };

    if (this->treeMembers[section].find(variable) == this->treeMembers[section].end())
    {
      cout << "V sekcii \"" << section << "\" net peremennoj \"" << variable << "\", a est:" << endl;

      for (auto const &[variable, value] : this->treeMembers[section])
      {
        cout << variable << endl;
      };

      throw invalid_argument("Net peremennoj \"" + variable + "\"");
      return universalResult;
    };

    string protoResult = this->treeMembers[section][variable];

    regex patternIntegerNumber("-?[0-9]+");
    regex patternDoubleNumber("-?[0-9]+\\.[0-9]+");
    regex patternStringLine("[\\s\\S]*[a-zA-Z][\\s\\S]*");

    if constexpr (is_same_v<T, int>)
    {
      if (regex_match(protoResult, patternIntegerNumber))
      {
        universalResult = stoi(protoResult);
        return universalResult;
      };
      throw invalid_argument("Oshibka identifikacii tipa!");
      return universalResult;
    };

    if constexpr (is_same_v<T, double>)
    {
      if (regex_match(protoResult, patternDoubleNumber))
      {
        universalResult = stod(protoResult);
        return universalResult;
      };
      throw invalid_argument("Oshibka identifikacii tipa!");
      return universalResult;
    };

    if constexpr (is_same_v<T, std::string>)
    {
      if (regex_match(protoResult, patternStringLine))
      {
        universalResult = protoResult;
        return universalResult;
      };
      throw invalid_argument("Oshibka identifikacii tipa!");
      return universalResult;
    };

    throw invalid_argument("Zadan nepredusmotrennyij tip!");
    return universalResult;
  };

  void cleansUpSpacesInBeginningAndInEnd(string &line)
  {
    line.erase(0, line.find_first_not_of(" \t\r\n\v\f")); // пробелы в начале
    line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1); // пробелы в конце
    return;
  }

private:
  map<string, map<string, string>> treeMembers; // дерево структуры;
};

int main()
{
  ini_parser parser("./config.ini");
  try
  {
    cout << parser.getValue<double>("Section1", "var1") << endl;
    cout << parser.getValue<string>("Section1", "var2") << endl;
    cout << parser.getValue<string>("Section1", "var3") << endl;
    cout << parser.getValue<int>("Section2", "var1") << endl;
    cout << parser.getValue<string>("Section2", "var2") << endl;
    cout << parser.getValue<int>("Section2", "var3") << endl;
    cout << parser.getValue<string>("Section4", "Mode") << endl;
    cout << parser.getValue<string>("Section4", "Vid") << endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }

  cout << endl;

  try
  {
    cout << parser.getValue<double>("Section10", "var10") << endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }

  cout << endl;

  try
  {
    cout << parser.getValue<double>("Section1", "var10") << endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }

  cout << endl;

  try
  {
    cout << parser.getValue<string>("Section1", "var1") << endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }

  cout << endl;

  try
  {
    cout << parser.getValue<bool>("Section1", "var1") << endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }

  cout << endl;

  try
  {
    ini_parser parser2("./configg.ini");
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }

  cout << endl;

  try
  {
    ini_parser parser3("./config2.ini");
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }

  return 0;
};
