// SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: BSD-3-Clause
#include <OpenXLSX.hpp>

#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void Check(bool condition, char const* name) {
   if (!condition)
      throw std::runtime_error(name);
   std::cout << "SMOKE|CHECK|" << name << "|PASS\n";
}
}

int main() {
   namespace fs = std::filesystem;
   fs::path const path = fs::current_path() / "buildengine-openxlsx-roundtrip.xlsx";
   try {
      {
         OpenXLSX::XLDocument document;
         document.create(path.string(), OpenXLSX::XLForceOverwrite);
         auto sheet = document.workbook().worksheet("Sheet1");
         sheet.cell("A1").value() = "Caption";
         sheet.cell("B1").value() = "Amount";
         sheet.cell("A2").value() = "Gr\xC3\xBC\xC3\x9Fe";
         sheet.cell("B2").value() = 42.125;
         sheet.cell("C2").value() = 123456;
         sheet.cell("D2").value() = true;
         document.save();
         document.close();
      }
      {
         OpenXLSX::XLDocument document;
         document.open(path.string());
         auto sheet = document.workbook().worksheet("Sheet1");
         Check(static_cast<std::string>(sheet.cell("A1").value()) == "Caption", "caption");
         Check(static_cast<std::string>(sheet.cell("B1").value()) == "Amount", "caption-2");
         Check(static_cast<std::string>(sheet.cell("A2").value()) == "Gr\xC3\xBC\xC3\x9Fe", "unicode");
         Check(std::abs(static_cast<double>(sheet.cell("B2").value()) - 42.125) < 1e-10, "decimal");
         Check(static_cast<int>(sheet.cell("C2").value()) == 123456, "integer");
         Check(static_cast<bool>(sheet.cell("D2").value()), "boolean");
         document.close();
      }
      fs::remove(path);
      std::cout << "SMOKE|RESULT|PASS|OpenXLSX workbook roundtrip\n";
      return 0;
   }
   catch (std::exception const& error) {
      std::cerr << "SMOKE|RESULT|FAIL|" << error.what() << '\n';
      return 1;
   }
}
