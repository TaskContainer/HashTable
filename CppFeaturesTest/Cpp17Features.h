#pragma once

/*
В VS2017 содержимое некоторых заголовочных файлов упакованно в пространство имён std::experimental::
По стандарту C++17 оно должно находиться в пространстве имён std::
Например, содержимое <filesystem> находится в std::experimental::filesystem, а не в std::filesystem::
*/

// На всякий случай (если в будущем его уберут) создаём пространство std::experimental::
namespace std::experimental {}

// Распаковываем содержимое std::experimental в std::
namespace std { using namespace experimental; }

// Теперь, например, содержимое <filesystem> будет находиться в std::filesystem::