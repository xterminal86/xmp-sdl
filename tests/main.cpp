#include "scroll-buffer.h"

#include <string>
#include <format>

int main()
{
  ScrollBuffer<std::string> scroll(10, 4);

  for (size_t i = 0; i < 20; i++)
  {
    std::string s = std::format("message {}", (i + 1));
    scroll.AddMessage(s);
  }

  scroll.SetScrollState(ScrollState::TOP);

  scroll.ScrollDown();

  auto v = scroll.GetMessages();
  for (std::string* s : v)
  {
    printf("'%s'\n", s->data());
  }

  return 0;
}
