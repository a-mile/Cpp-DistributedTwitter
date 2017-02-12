C++ Rozproszony Tweeter

Opis:

Program napisano w oparciu o protokół TCP/IP korzystając z interfejsu gniazd BSD. Składa się on z dwóch części: 
- serwera, który umożliwia zapisywanie wiadomości od właściciela oraz odczytywanie wiadomości przez wszystkich użytkowników, 
- klienta, który służy do komunikacji z serwerami. Umożliwia pisanie wiadomości do własnego serwera oraz odczytywanie 
wiadomości z dowolnego serwera. Pozwala na zarządzanie serwerami (dodawanie i usuwanie)

Sposób kompilacji:

Program korzysta z systemu budowania cmake.

Sposób użycia:

Sterowanie programem odbywa się za pomocą konsoli, należy podać numer opcji, która nas interesuję i zatwierdzić enterem. 
Program posiada częściową obsługę sytuacji wyjątkowych, tzn. nie można wybrać nie istniejącej opcji, a niedozwolone znaki 
zostaną odrzucone.

Skompilowane programy klienta i serwera należy umieścić w jednym folderze. Serwer przy pierwszym uruchomieniu wymaga 
konfiguracji - należy podać jego nazwę oraz port, z którym będzie się komunikował, informacje zostają zapisane w pliku. 
Następnie można uruchomić klienta, który umożliwia napisanie nowego postu na własnym serwerze, odczytanie postów z dostępnych
serwerów oraz zarządzanie serwerami. Za pomocą opcji zarządzania serwerami możemy dodać nowy serwer podając jego nazwę, ip 
oraz port, informacje zostają zapisane w pliku. Można także bez problemu usunąć dodany serwer. W przypadku, gdy klient nie 
może się połączyć z danym serwerem wyświetla stosowną informację. Warto wspomnieć, że klient umożliwia także pracę bez 
uruchomionego własnego serwera, w tym przypadku nie ma możliwości pisania wiadomości.
