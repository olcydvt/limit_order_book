# Limit Order Book


## Installation

NA

## Usage

```c++
#include "order_book.hpp"


int main()
{

    order_book order_book;

    //order_book.place_order(1, -100, 10); //short
    //order_book.place_order(2, -101, 15); //short
    //order_book.place_order(3, 101, 30);  //long

    order_book.place_order(1, -101, 10); //short
    order_book.place_order(2, 99, 15);   //long
    order_book.place_order(3, -99, 30);  //short

    return 0;
    
}
```

## Contributing

NA

## License

[MIT](https://choosealicense.com/licenses/mit/)
