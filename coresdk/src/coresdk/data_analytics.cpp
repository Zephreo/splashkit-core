#include "data_analytics.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <regex>
#include "terminal.h"
#include "basics.h"

using namespace std;

namespace splashkit_lib
{
    // Save data into current file
    void save_file(string original_file_path, string data)
    {
        ofstream file(original_file_path);

        if (file.is_open())
        {
            file << data;
            file.close();
        }
        else
            cout << "Unable to save file";
    }

    // Input validation functions
    bool input_has_symbol(string input)
    {
        return regex_match(input, symbols_pattern);
    }

    bool input_has_alphabet_char(string input)
    {
        return regex_match(input, ABC_pattern);
    }

    bool input_is_valid_url(string input)
    {
        return regex_match(input, URL_pattern);
    }

    bool input_is_email(string input)
    {
        return regex_match(input, email_pattern);
    }

    bool input_is_phone_number(string input)
    {
        return regex_match(input, phone_number_pattern);
    }

    bool input_is_time(string input)
    {
        return regex_match(input, time_pattern);
    }

    // Run all validation checks attempting to identify and return input type
    int identify_input_type(string input)
    {
        if (input_is_phone_number(input))
            return 1;
        else if (input_is_email(input))
            return 2;
        else if (input_is_time(input))
            return 3;
        else if (input_is_valid_url(input))
            return 4;
        else
            return -1;
    }

    /* #region DataFrames */

    /* #region  Constructors */
    // Import from Vector
    template <typename T>
    DataFrame<T>::DataFrame(vector<string> _tempHeader, bool _index) : m_Columns((_index) ? _tempHeader.size() - 1 : _tempHeader.size()), m_Index(_index)
    {
        if (_index)
        {
            m_HeaderNames = _tempHeader;
        }
        else
        {
            _tempHeader.insert(_tempHeader.begin(), "Index");
            m_HeaderNames = _tempHeader;
        }
    };

    template <typename T>
    DataFrame<T>::DataFrame(const DataFrame &&_other) : m_Columns(_other.m_Columns), m_Rows(_other.m_Rows), m_Index(_other.m_Index), m_HeaderNames(_other.m_HeaderNames), m_Table(_other.m_Table), m_IndexNames(_other.m_IndexNames){};
    template <typename T>
    DataFrame<T>::DataFrame(DataFrame &_other) : m_Columns(_other.m_Columns), m_Rows(_other.m_Rows), m_Index(_other.m_Index), m_HeaderNames(_other.m_HeaderNames), m_Table(_other.m_Table), m_IndexNames(_other.m_IndexNames){};

    // Import from Array
    // Import from R-Value
    template <typename T>
    DataFrame<T>::DataFrame(unsigned int _columns, const string (&_tempHeader)[], bool _index) : m_Columns(_columns), m_Index(_index)
    {
        if (_index)
        {
            for (size_t i = 0; i < m_Columns; i++)
            {
                m_HeaderNames.push_back(std::move(_tempHeader[i]));
            };
            m_Columns--;
        }
        else
        {
            m_HeaderNames.push_back("Index");
            for (size_t i = 0; i < m_Columns; i++)
            {
                m_HeaderNames.push_back(std::move(_tempHeader[i]));
            };
        }
    };
    // Import From L-Value
    template <typename T>
    DataFrame<T>::DataFrame(unsigned int _columns, const string(_tempHeader)[], bool _index) : m_Columns(_columns), m_Index(_index)
    {
        if (_index)
        {
            for (size_t i = 0; i < m_Columns; i++)
            {
                m_HeaderNames.push_back(std::move(_tempHeader[i]));
            };
            m_Columns--;
        }
        else
        {
            m_HeaderNames.push_back("Index");
            for (size_t i = 0; i < m_Columns; i++)
            {
                m_HeaderNames.push_back(std::move(_tempHeader[i]));
            };
        }
    };

    // Import From TextFile
    template <>
    DataFrame<string>::DataFrame(string _textFileName, bool _index) : m_Index(_index)
    {
        // Open file
        fstream newFile;
        newFile.open(_textFileName, ios::in);
        if (newFile.is_open())
        {
            string newLine;
            bool firstLine = true;
            // Get line from file, count commas to get m_Columns size
            while (getline(newFile, newLine))
            {
                if (newLine.length() == 0)
                {
                    break;
                }
                if (m_Columns == 0)
                {
                    for (int i = 0; i < newLine.size(); i++)
                    {
                        if (newLine[i] == ',')
                        {
                            m_Columns++;
                        }
                    }
                    m_Columns++;
                    if (!m_Index)
                    {
                        m_HeaderNames.push_back("Index");
                    }
                }

                // firstIndex is solely for if m_Index is true

                stringstream ss(newLine);
                // Use to seperate data in csv file to insert into m_Table

                while (ss.good())
                {
                    string substr;

                    // ERASE SPACES
                    for (int i = 0; i < m_Columns; i++)
                    {

                        getline(ss, substr, ',');
                        substr.erase(remove(substr.begin(), substr.end(), ' '), substr.end());

                        // cout << substr;
                        if (firstLine)
                        {
                            m_HeaderNames.push_back(substr);
                            // cout << "Header";
                        }
                        else
                        {

                            if (m_Index == true && i == 0)
                            {
                                m_IndexNames.push_back(substr);
                                // cout << "Index";
                            }
                            else
                            {
                                m_Table.push_back(substr);
                            }
                        }
                    }

                    firstLine = false;
                    m_Rows++;
                }
            }
        }
        newFile.close();
        m_Rows--;
        if (!m_Index)
        {
            for (int i = 0; i < m_Rows; i++)
            {
                // cout << i;
                m_IndexNames.push_back(to_string(i));
            }
        }
        if (m_Index)
        {
            m_Columns--;
        }
    };

    template <typename T>
    DataFrame<T>::DataFrame(string _textFileName, bool _index) : m_Index(_index)
    {
        // Open file
        fstream newFile;
        newFile.open(_textFileName, ios::in);
        if (newFile.is_open())
        {
            string newLine;
            bool firstLine = true;
            // Get line from file, count commas to get columns size
            while (getline(newFile, newLine))
            {
                if (newLine.length() == 0)
                {
                    break;
                }

                if (m_Columns == 0)
                {
                    for (int i = 0; i < newLine.size(); i++)
                    {
                        if (newLine[i] == ',')
                        {
                            m_Columns++;
                        }
                    }
                    m_Columns++;
                    if (!m_Index)
                    {
                        m_HeaderNames.push_back("Index");
                    }
                }

                // firstIndex is solely for if m_Index is true

                stringstream ss(newLine);
                // Use to seperate data in csv file to insert into m_Table
                while (ss.good())
                {
                    string substr;

                    // ERASE SPACES
                    for (int i = 0; i < m_Columns; i++)
                    {
                        getline(ss, substr, ',');
                        substr.erase(remove(substr.begin(), substr.end(), ' '), substr.end());
                        // cout << substr;
                        if (firstLine)
                        {
                            m_HeaderNames.push_back(substr);
                            // cout << "Header";
                        }
                        else
                        {

                            if (m_Index == true && i == 0)
                            {
                                m_IndexNames.push_back(substr);
                                // cout << "Index";
                            }
                            else
                            {
                                m_Table.push_back((T)stod(substr));
                            }
                        }
                    }
                    firstLine = false;
                    m_Rows++;
                }
            }
        }
        newFile.close();
        m_Rows--;
        if (!m_Index)
        {
            for (int i = 0; i < m_Rows; i++)
            {
                // cout << i;
                m_IndexNames.push_back(to_string(i));
            }
        }
        if (m_Index)
        {
            m_Columns--;
        }
    };
    /* #endregion */

    // Members

    // Export To File

    template <typename T>
    const void DataFrame<T>::exportToFile(string _fileName, flags _eType) const
    {
        ofstream fileName;
        if (_eType == 0x01)
        {
            fileName.open(_fileName, ios::out);
            for (size_t i = 0; i < m_Columns + 1; i++)
            {
                fileName << m_HeaderNames[i];
                if (i != m_Columns)
                    fileName << ", ";
                else
                    fileName << endl;
            }
        }
        else if (_eType == 0x02)
        {
            fileName.open(_fileName, ios::app);
        }
        else
        {
            cout << "Incorrect Flag to Export" << endl;
            return;
        }

        for (size_t i = 0; i < m_Rows; i++)
        {
            ostringstream oss;
            oss << m_IndexNames[i] << ", ";
            for (size_t j = 0; j < m_Columns; j++)
            {
                oss << m_Table[i * m_Columns + j];
                if (j != m_Columns - 1)
                {
                    oss << ", ";
                };
            }
            fileName << oss.str() << endl;
        }
    };

    // Push Back Functions
    // Pass By Reference (Takes as R-Value)
    template <typename T>
    void DataFrame<T>::push_back(const string (&_arr)[], unsigned int _length)
    {
        if (_length == m_Columns && !m_Index)
        {
            m_IndexNames.push_back(to_string(m_Rows));
            for (int i = 0; i < m_Columns; i++)
            {
                m_Table.push_back((T)stod(std::move(_arr[i])));
            }
            m_Rows += 1;
        }
        else if ((_length == m_Columns + 1) && m_Index)
        {
            m_IndexNames.push_back(_arr[0]);
            for (int i = 1; i < m_Columns + 1; i++)
            {
                m_Table.push_back((T)stod(std::move(_arr[i])));
            }
            m_Rows += 1;
        }
        else
        {
            cout << "Index Set to False/Array Out Of Range" << endl;
        }
    };

    template <>
    void DataFrame<string>::push_back(const string (&_arr)[], unsigned int _length)
    {
        if (_length == m_Columns && !m_Index)
        {
            m_IndexNames.push_back(to_string(m_Rows));
            for (int i = 0; i < m_Columns; i++)
            {
                m_Table.push_back(std::move(_arr[i]));
            }
            m_Rows += 1;
        }
        else if ((_length == m_Columns + 1) && m_Index)
        {
            m_IndexNames.push_back(_arr[0]);
            for (int i = 1; i < m_Columns + 1; i++)
            {
                m_Table.push_back(std::move(_arr[i]));
            }
            m_Rows += 1;
        }
        else
        {
            cout << "Index Set to False/Array Out Of Range" << endl;
        }
    };

    // Takes As L-Value
    template <typename T>
    void DataFrame<T>::push_back(const string(_arr)[], unsigned int _length)
    {
        if (_length == m_Columns && !m_Index)
        {
            m_IndexNames.push_back(to_string(m_Rows));
            for (int i = 0; i < m_Columns; i++)
            {
                m_Table.push_back((T)stod((_arr[i])));
            }
            m_Rows += 1;
        }
        else if ((_length == m_Columns + 1) && m_Index)
        {
            m_IndexNames.push_back(_arr[0]);
            for (int i = 1; i < m_Columns + 1; i++)
            {
                m_Table.push_back((T)stod((_arr[i])));
            }
            m_Rows += 1;
        }
        else
        {
            cout << "Index Set to False/Array Out Of Range" << endl;
        }
    };

    template <>
    void DataFrame<string>::push_back(const string(_arr)[], unsigned int _length)
    {
        if (_length == m_Columns && !m_Index)
        {
            m_IndexNames.push_back(to_string(m_Rows));
            for (int i = 0; i < m_Columns; i++)
            {
                m_Table.push_back((_arr[i]));
            }
            m_Rows += 1;
        }
        else if ((_length == m_Columns + 1) && m_Index)
        {
            m_IndexNames.push_back(_arr[0]);
            for (int i = 1; i < m_Columns + 1; i++)
            {
                m_Table.push_back((_arr[i]));
            }
            m_Rows += 1;
        }
        else
        {
            cout << "Index Set to False/Array Out Of Range" << endl;
        }
    };

    template <typename T>
    void DataFrame<T>::push_back(const DataFrame<T> _dFrame)
    {
        if (m_Columns == _dFrame.m_Columns && m_Index == _dFrame.m_Index)
        {
            m_Rows += _dFrame.m_Rows;
            m_Table.insert(m_Table.end(), _dFrame.m_Table.begin(), _dFrame.m_Table.end());
            m_IndexNames.insert(m_IndexNames.end(), _dFrame.m_IndexNames.begin(), _dFrame.m_IndexNames.end());
        }
        else
        {
            cout << "Can't Merge due to inequal columns and/or index" << endl;
        }
    };

    // Insert Functions
    // Pass By Reference (Takes as R-Value)
    // Need to check POs
    template <typename T>
    void DataFrame<T>::insert(const string (&_arr)[], unsigned int _length, unsigned int _pos)
    {
        T temp[_length] = {};
        if (_length == m_Columns && !m_Index)
        {
            m_IndexNames.insert(m_IndexNames.begin() + _pos, to_string(m_Rows));
            m_Table.insert(m_Table.begin() + (m_Columns * _pos), temp, temp + _length);
            for (int i = 0; i < m_Columns; i++)
            {
                m_Table[m_Columns * _pos + i] = (T)stod(_arr[i]);
            }
            m_Rows += 1;
        }
        else if ((_length == m_Columns + 1) && m_Index)
        {
            m_IndexNames.insert(m_IndexNames.begin() + _pos, _arr[0]);
            m_Table.insert(m_Table.begin() + (m_Columns * _pos), temp, temp + _length - 1);
            for (int i = 1; i < m_Columns + 1; i++)
            {
                m_Table[m_Columns * _pos + i - 1] = (T)stod(_arr[i]);
            }
            m_Rows += 1;
        }
        else
        {
            cout << "Index Set to False/Array Out Of Range" << endl;
        }
    };

    template <>
    void DataFrame<string>::insert(const string (&_arr)[], unsigned int _length, unsigned int _pos)
    {
        string temp[_length] = {};
        if (_length == m_Columns && !m_Index)
        {
            m_Table.insert(m_Table.begin() + (m_Columns * _pos), temp, temp + _length);
            m_IndexNames.insert(m_IndexNames.begin() + _pos, to_string(m_Rows));
            for (int i = 0; i < m_Columns; i++)
            {
                m_Table[m_Columns * _pos + i] = _arr[i];
            }
            m_Rows += 1;
        }
        else if ((_length == m_Columns + 1) && m_Index)
        {
            m_Table.insert(m_Table.begin() + (m_Columns * _pos), temp, temp + _length - 1);
            m_IndexNames.insert(m_IndexNames.begin() + _pos, _arr[0]);
            for (int i = 1; i < m_Columns + 1; i++)
            {
                m_Table[m_Columns * _pos + i - 1] = _arr[i];
            }
            m_Rows += 1;
        }
        else
        {
            cout << "Index Set to False/Array Out Of Range" << endl;
        }
    };

    // Takes As L-Value
    template <typename T>
    void DataFrame<T>::insert(const string(_arr)[], unsigned int _length, unsigned int _pos)
    {
        T temp[_length] = {};
        if (_length == m_Columns && !m_Index)
        {
            m_IndexNames.insert(m_IndexNames.begin() + _pos, to_string(m_Rows));
            m_Table.insert(m_Table.begin() + (m_Columns * _pos), temp, temp + _length);
            for (int i = 0; i < m_Columns; i++)
            {
                m_Table[m_Columns * _pos + i] = (T)stod(_arr[i]);
            }
            m_Rows += 1;
        }
        else if ((_length == m_Columns + 1) && m_Index)
        {
            m_IndexNames.insert(m_IndexNames.begin() + _pos, _arr[0]);
            m_Table.insert(m_Table.begin() + (m_Columns * _pos), temp, temp + _length - 1);

            for (int i = 1; i < m_Columns + 1; i++)
            {
                m_Table[m_Columns * _pos + i - 1] = (T)stod(_arr[i]);
            }
            m_Rows += 1;
        }
        else
        {
            cout << "Index Set to False/Array Out Of Range" << endl;
        }
    }

    template <>
    void DataFrame<string>::insert(const string(_arr)[], unsigned int _length, unsigned int _pos)
    {
        string temp[_length] = {};
        if (_length == m_Columns && !m_Index)
        {
            m_Table.insert(m_Table.begin() + (m_Columns * _pos), temp, temp + _length);
            m_IndexNames.insert(m_IndexNames.begin() + _pos, to_string(m_Rows));
            for (int i = 0; i < m_Columns; i++)
            {
                m_Table[m_Columns * _pos + i] = _arr[i];
            }
            m_Rows += 1;
        }
        else if ((_length == m_Columns + 1) && m_Index)
        {
            m_Table.insert(m_Table.begin() + (m_Columns * _pos), temp, temp + _length - 1);
            m_IndexNames.insert(m_IndexNames.begin() + _pos, _arr[0]);
            for (int i = 1; i < m_Columns + 1; i++)
            {
                m_Table[m_Columns * _pos + i - 1] = _arr[i];
            }
            m_Rows += 1;
        }
        else
            cout << "Index Set to False/Array Out Of Range" << endl;
    }

    template <typename T>
    void DataFrame<T>::insert(const DataFrame<T> _dFrame, unsigned int _pos)
    {
        if (m_Columns == _dFrame.m_Columns && m_Index == _dFrame.m_Index)
        {
            m_Rows += _dFrame.m_Rows;
            m_Table.insert(m_Table.begin() + (_pos * m_Columns), _dFrame.m_Table.begin(), _dFrame.m_Table.end());
            m_IndexNames.insert(m_IndexNames.begin() + (_pos * m_Columns), _dFrame.m_IndexNames.begin(), _dFrame.m_IndexNames.end());
        }
        else
            cout << "Can't Merge due to unequal columns and/or index" << endl;
    }

    // Insert Column
    template <typename T>
    void DataFrame<T>::insertC(const string _headerName, const string defaultValue)
    {
        m_HeaderNames.push_back(_headerName);
        for (int i = 0; i < m_Rows; i++)
            m_Table.insert(m_Table.begin() + (m_Columns + ((m_Columns + 1) * i)), stod(defaultValue));
        m_Columns++;
    }

    // Delete Row

    template <typename T>
    void DataFrame<T>::deleteRow(unsigned int _row)
    {
        if (_row <= m_Rows - 1)
        {
            m_Table.erase(m_Table.begin() + (m_Columns * _row), m_Table.begin() + ((m_Columns * (_row + 1))));
        }
        else
        {
            cout << "Index out of Range" << endl;
            return;
        }
        m_IndexNames.erase(m_IndexNames.begin() + _row);
        m_Rows--;
    }

    // Delete Column

    template <typename T>
    void DataFrame<T>::deleteColumn(unsigned int _column)
    {
        if (_column >= m_Columns)
        {
            cout << "Column out of range" << endl;
            return;
        }
        vector<T> tempTable;
        for (int i = 0; i < m_Table.size(); i++)
            if (!(sMod(i - _column, m_Columns) == 0))
                tempTable.push_back(m_Table[i]);
        m_Table = tempTable;
        m_Columns--;
        m_HeaderNames.erase(m_HeaderNames.begin() + _column + 1);
    }

    // Get Row

    template <typename T>
    const DataFrame<T> DataFrame<T>::getRow(unsigned int _row) const
    {
        DataFrame<T> temp(m_HeaderNames, m_Index);
        temp.m_Table.insert(temp.m_Table.begin(), m_Table.begin() + (_row * m_Columns), m_Table.begin() + (_row * m_Columns + m_Columns));
        temp.m_Rows = 1;
        temp.m_IndexNames.insert(temp.m_IndexNames.begin(), m_IndexNames[_row]);
        return temp;
    }

    // Get Column

    template <typename T>
    const DataFrame<T> DataFrame<T>::getColumn(unsigned int _column) const
    {
        DataFrame<T> temp(2, {m_HeaderNames[0], m_HeaderNames[_column + 1]}, m_Index);
        if (!m_Index)
            temp.m_HeaderNames[0] = "Index";

        temp.m_Rows = m_Rows;
        temp.m_IndexNames = m_IndexNames;

        for (size_t i = 0; i < m_Rows; i++)
            temp.m_Table.push_back(m_Table[_column + (m_Columns * i)]);

        return temp;
    }

    template <typename T>
    const DataFrame<T> DataFrame<T>::getColumn(string _column) const
    {
        auto tempHead = find(m_HeaderNames.begin(), m_HeaderNames.end(), _column);
        int index = tempHead - m_HeaderNames.begin();
        if (tempHead == m_HeaderNames.end())
        {
            std::cout << "Could Not Find Header Name" << std::endl;
            std::exit(0);
        }
        DataFrame<T> temp(2, {m_HeaderNames[0], m_HeaderNames[index]}, m_Index);
        if (m_Index)
            index--;
        else
            temp.m_HeaderNames[0] = "Index";
        temp.m_Rows = m_Rows;
        temp.m_IndexNames = m_IndexNames;
        for (size_t i = 0; i < m_Rows; i++)
            temp.m_Table.push_back(m_Table[index + (m_Columns * i)]);
        return temp;
    }

    // Transpose
    template <typename T>
    void DataFrame<T>::transpose()
    {
        T tempArray[(m_Rows) * (m_Columns)];
        // cout << (m_Rows)*(m_Columns) << endl;
        for (int i = 0; i < m_Columns; i++)
        {
            // cout << "Line" << endl;
            for (int j = 0; j < m_Rows; j++)
            {
                tempArray[((m_Rows)*i) + j] = std::move(m_Table[j * (m_Columns) + i]);
                // cout << tempArray[((m_Rows-1) * i) + j] << endl;
                // cout << i << endl;
                // cout << j << endl;
                // cout << ((m_Rows-1) * i) + j << endl;
            }
        }
        m_Table.clear();
        for (int i = 0; i < (m_Rows) * (m_Columns); i++)
        {
            m_Table.push_back(tempArray[i]);
        }
        m_IndexNames.insert(m_IndexNames.begin(), m_HeaderNames[0]);
        m_HeaderNames.erase(m_HeaderNames.begin());
        m_IndexNames.swap(m_HeaderNames);
        int temp = m_Rows;
        m_Rows = m_Columns;
        m_Columns = temp;
    }

    // Swap

    template <typename T>
    void DataFrame<T>::swap(unsigned int _index1, unsigned int _index2)
    {
        std::swap_ranges(m_Table.begin() + (_index1 * m_Columns), m_Table.begin() + (_index1 * m_Columns) + m_Columns, m_Table.begin() + (_index2 * m_Columns));
        std::swap(m_IndexNames[_index1], m_IndexNames[_index2]);
    }

    // Reset Index to numbers
    template <typename T>
    void DataFrame<T>::resetIndex()
    {
        for (size_t i = 0; i < m_Columns; i++)
            m_IndexNames[i] = to_string(i);
    }
    /* #endregion */
}