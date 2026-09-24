#include <pqxx/util.hxx>

#include <print>

int main()
{
   auto const theModel = pqxx::describe_thread_safety();
   std::println("libpqxx: {}", theModel.description);
   return theModel.safe_libpq ? 0 : 1;
}
