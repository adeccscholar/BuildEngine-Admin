#include <pqxx/util>

#include <print>

int main()
{
   auto const theModel = pqxx::describe_thread_safety();

   if(theModel.safe_libpq)
      {
      std::println("SMOKE|CHECK|thread-safety|PASS|{}", theModel.description);
      std::println("SMOKE|RESULT|PASS|libpqxx consumer smoke passed");
      return 0;
      }

   std::println("SMOKE|CHECK|thread-safety|FAIL|{}", theModel.description);
   std::println("SMOKE|RESULT|FAIL|libpqxx consumer smoke failed");
   return 1;
}
