// Copyright (c) 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: MIT
// Project: adecc Scholar

#include <cmark-gfm-core-extensions.h>
#include <cmark-gfm-extension_api.h>
#include <cmark-gfm.h>

#include <array>
#include <cstdio>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

   struct cmark_text_deleter {
      void operator()(char* szText) const noexcept {
         if (szText != nullptr) {
            cmark_get_default_mem_allocator()->free(szText);
         }
      }
   };

   using cmark_parser_ptr = std::unique_ptr<cmark_parser, decltype(&cmark_parser_free)>;
   using cmark_node_ptr = std::unique_ptr<cmark_node, decltype(&cmark_node_free)>;
   using cmark_text_ptr = std::unique_ptr<char, cmark_text_deleter>;

   void AttachExtension(cmark_parser* theParser, char const* szName) {
      cmark_syntax_extension* theExtension = cmark_find_syntax_extension(szName);
      if (theExtension == nullptr) {
         throw std::runtime_error(std::string { "Missing GFM extension: " } + szName);
      }

      if (!cmark_parser_attach_syntax_extension(theParser, theExtension)) {
         throw std::runtime_error(std::string { "Cannot attach GFM extension: " } + szName);
      }
   }

   void RequireContains(std::string_view svHtml, std::string_view svNeedle, char const* szFeature) {
      if (svHtml.find(svNeedle) == std::string_view::npos) {
         std::fputs("Rendered HTML:\n", stderr);
         std::fwrite(svHtml.data(), 1, svHtml.size(), stderr);
         std::fputc('\n', stderr);
         throw std::runtime_error(std::string { "Rendered HTML does not demonstrate " } + szFeature);
      }
   }

} // namespace

int main() {
   try {
      cmark_gfm_core_extensions_ensure_registered();

      cmark_parser_ptr theParser { cmark_parser_new(CMARK_OPT_DEFAULT), &cmark_parser_free };
      if (!theParser) {
         throw std::runtime_error("cmark_parser_new failed");
      }

      std::array<char const*, 5> const arrExtensions {
         "table",
         "strikethrough",
         "autolink",
         "tagfilter",
         "tasklist"
      };

      for (char const* szExtension : arrExtensions) {
         AttachExtension(theParser.get(), szExtension);
      }

      char const szMarkdown[] =
         "# BCC64X cmark-gfm smoke\n\n"
         "| Library | Status |\n"
         "| --- | --- |\n"
         "| libcmark-gfm | ~~legacy~~ green |\n\n"
         "- [x] Core library\n"
         "- [ ] GFM extensions\n\n"
         "Visit https://example.com for an autolink.\n\n"
         "<em>tagfilter control</em>\n\n"
         "<xmp>tagfilter probe</xmp>\n";

      cmark_parser_feed(theParser.get(), szMarkdown, std::strlen(szMarkdown));
      cmark_node_ptr theDocument { cmark_parser_finish(theParser.get()), &cmark_node_free };
      if (!theDocument) {
         throw std::runtime_error("cmark_parser_finish failed");
      }

      cmark_llist* theExtensions = cmark_parser_get_syntax_extensions(theParser.get());
      cmark_text_ptr theHtml { cmark_render_html(theDocument.get(), CMARK_OPT_UNSAFE, theExtensions) };
      if (!theHtml) {
         throw std::runtime_error("cmark_render_html failed");
      }

      std::string const strHtml { theHtml.get() };
      RequireContains(strHtml, "<table>", "table extension");
      RequireContains(strHtml, "<del>legacy</del>", "strikethrough extension");
      RequireContains(strHtml, "https://example.com", "autolink extension");
      RequireContains(strHtml, "<input type=\"checkbox\" checked=\"\" disabled=\"\" />", "checked tasklist item");
      RequireContains(strHtml, "<input type=\"checkbox\" disabled=\"\" />", "unchecked tasklist item");
      RequireContains(strHtml, "<em>tagfilter control</em>", "raw HTML control");
      RequireContains(strHtml, "&lt;xmp>", "tagfilter extension");

      std::printf("cmark-gfm BCC64X smoke passed.\n");
      std::printf("Extensions: table, strikethrough, autolink, tagfilter, tasklist\n");
      std::printf("Rendered HTML bytes: %zu\n", strHtml.size());
      return 0;
   }
   catch (std::exception const& theException) {
      std::fprintf(stderr, "ERROR: %s\n", theException.what());
      return 1;
   }
}
