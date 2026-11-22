// SPDX-License-Identifier: MIT
// DXC original TAEF XML-row replacement with standard C++23 data access.
// Source: original Microsoft DXC v1.9.2609 ShaderOpArith*.xml.
#include "dxc/Test/WEXAdapter.h"
#include "PortableTestDataGenerated.h"
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>

namespace WEX::TestExecution {
std::vector<std::wstring> LookupTestDataValues(
    wchar_t const* szField, bool& bFound) {
  bFound = false;
  char const* szTable = std::getenv("ADECC_DXC_TEST_TABLE");
  char const* szRow = std::getenv("ADECC_DXC_TEST_ROW");
  if (!szTable || !szRow || !szField)
    return {};

  // Names in the original XML tables are ASCII metadata; parameter values
  // are kept as wide strings and are not re-encoded by this lookup.
  std::wstring strTable, strRow;
  for (; *szTable; ++szTable)
    strTable += static_cast<unsigned char>(*szTable);
  for (; *szRow; ++szRow)
    strRow += static_cast<unsigned char>(*szRow);

  std::vector<std::wstring> vecValues;
  for (auto const& theEntry : adecc_dxc_test_data::entries) {
    if (strTable == theEntry.table &&
        strRow == theEntry.row &&
        std::wstring_view(szField) == theEntry.field) {
      bFound = true;
      vecValues.emplace_back(theEntry.value);
    }
  }
  return vecValues;
}
} // namespace WEX::TestExecution
