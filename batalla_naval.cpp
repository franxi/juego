#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

// Casilla del tablero
class Casilla
{
public:
  bool ocupada = false; // Indica si la casilla está ocupada por un barco
  bool atacada = false; // Indica si la casilla ha sido atacada
  char tipo = '~';      // Representa el tipo de barco en la casilla (o un espacio si no hay barco)
};

// Barco del juego
class Barco
{
public:
  char tipo;              // Tipo de barco ej: [S][L]
  int tamano;             // Tamaño del barco ej: [1][2]
  int filaInicial = 0;    // Coordenada inicial de la fila del barco
  int columnaInicial = 0; // Coordenada inicial de la columna del barco
  bool vertical = false;  // Dirección del barco (vertical u horizontal)

  // Constructor de la clase Barco define el tipo y tamano del barco
  Barco(char t, int tam) : tipo(t), tamano(tam) {}

  // Genera una posición aleatoria para el barco en el tablero
  void generarPosicion(int tamanoTablero)
  {
    srand(time(0));
    if (rand() % 2 == 0)
    {
      vertical = true;
      filaInicial = rand() % (tamanoTablero - tamano + 1);
      columnaInicial = rand() % tamanoTablero;
    }
    else
    {
      vertical = false;
      filaInicial = rand() % tamanoTablero;
      columnaInicial = rand() % (tamanoTablero - tamano + 1);
    }
  }
};

// Tablero del juego
class Tablero
{
public:
  int tamano;                                 // Tamaño del tablero
  std::vector<std::vector<Casilla>> casillas; // Matriz de casillas que representa el tablero
  // Constructor que inicializa el tablero con el tamano especificado
  Tablero(int tam) : tamano(tam), casillas(tam, std::vector<Casilla>(tam)) {}

  // Genera una posición aleatoria para el barco en el tablero
  void colocarBarcos(std::vector<Barco> &barcos)
  {
    for (Barco &barco : barcos)
    {
      do
      {
        barco.generarPosicion(tamano);
      } while (!verificarPosicion(barco));

      colocarBarco(barco);
    }
  }

  // Verifica si la posición generada para un barco es válida (no se superpone con otros barcos)
  bool verificarPosicion(const Barco &barco)
  {
    int desplazamientoMaximo = barco.vertical ? barco.filaInicial + barco.tamano : barco.columnaInicial + barco.tamano;
    for (int i = barco.vertical ? barco.filaInicial : barco.columnaInicial; i < desplazamientoMaximo; ++i)
    {
      if (barco.vertical && casillas[i][barco.columnaInicial].ocupada)
      {
        return false;
      }
      else if (!barco.vertical && casillas[barco.filaInicial][i].ocupada)
      {
        return false;
      }
    }
    return true;
  }

  // Coloca un barco en el tablero
  void colocarBarco(const Barco &barco)
  {
    int desplazamientoMaximo = barco.vertical ? barco.filaInicial + barco.tamano : barco.columnaInicial + barco.tamano;
    for (int i = barco.vertical ? barco.filaInicial : barco.columnaInicial; i < desplazamientoMaximo; ++i)
    {
      if (barco.vertical)
      {
        casillas[i][barco.columnaInicial] = {true, false, barco.tipo};
      }
      else
      {
        casillas[barco.filaInicial][i] = {true, false, barco.tipo};
      }
    }
  }

  // Realiza un ataque en la posición dada
  bool ataque(int fila, int columna)
  {

    if (fila < 0 || fila >= tamano || columna < 0 || columna >= tamano)
    {
      return false;
    }

    Casilla &casilla = casillas[fila][columna];
    if (casilla.atacada)
    {
      return false;
    }

    casilla.atacada = true;
    if (casilla.ocupada)
    {
      casilla.tipo = 'X'; // Marca un ataque exitoso
      return true;
    }
    else
    {
      casilla.tipo = 'O'; // Marca un ataque fallido
      return false;
    }
  }

  // Verifica si todos los barcos han sido hundidos
  bool todosHundidos() const
  {
    for (const auto &fila : casillas)
    {
      for (const auto &casilla : fila)
      {
        if (casilla.ocupada && !casilla.atacada)
        {
          return false;
        }
      }
    }
    return true;
  }

  // Imprime el tablero, mostrando solo las casillas que han sido atacadas
  void imprimirTablero() const
  {
    std::cout << "  ";
    for (int j = 1; j <= tamano; j++)
    {
      std::cout << j << " ";
    }
    std::cout << "\n";

    for (int i = 0; i < tamano; i++)
    {
      std::cout << static_cast<char>('A' + i) << " ";
      for (int j = 0; j < tamano; j++)
      {
        if (j < 9)
        {

          std::cout << (casillas[i][j].atacada ? casillas[i][j].tipo : '~') << " ";
        }
        else
        {
          std::cout << (casillas[i][j].atacada ? casillas[i][j].tipo : '~') << "  ";
        }
      }
      std::cout << "\n";
    }
  }

  // Imprime el tablero, mostrando la posición de todos los barcos
  void imprimirTableroConBarcos() const
  {
    std::cout << "  ";
    for (int j = 1; j <= tamano; j++)
    {
      std::cout << j << " ";
    }
    std::cout << "\n";

    for (int i = 0; i < tamano; i++)
    {
      std::cout << static_cast<char>('A' + i) << " ";
      for (int j = 0; j < tamano; j++)
      {
        if (j < 9)
        {

          std::cout << casillas[i][j].tipo << " ";
        }
        else
        {
          std::cout << casillas[i][j].tipo << "  ";
        }
      }
      std::cout << "\n";
    }
  }
};

// Función que realiza un movimiento aleatorio de la IA
void moverIA(Tablero &tablero)
{
  int fila, columna;
  do
  {
    fila = rand() % tablero.tamano;
    columna = rand() % tablero.tamano;
  } while (tablero.casillas[fila][columna].atacada);

  tablero.ataque(fila, columna);
}

// Convierte el input del jugador en coordenadas para el tablero
bool obtenerCoordenadas(std::string entrada, int &fila, int &columna)
{
  if (entrada.length() < 2)
  {
    return false;
  }

  char letra = toupper(entrada[0]);
  if (letra < 'A' || letra >= 'A' + 15)
  {
    return false;
  }
  fila = letra - 'A';

  int numero = std::stoi(entrada.substr(1)) - 1;
  if (numero < 0 || numero >= 15)
  {
    return false;
  }
  columna = numero;

  return true;
}

// Función principal
int main()
{
  srand(time(0));

  const int tamanoTablero = 15;
  Tablero tableroJugador(tamanoTablero);
  Tablero tableroIA(tamanoTablero);

  std::vector<Barco> barcosJugador = {
      // Barco('P', 5),
      // Barco('B', 4),
      // Barco('B', 4),
      // Barco('S', 3),
      Barco('S', 3),
      Barco('L', 1),
      Barco('L', 1),
      Barco('L', 1)};

  std::vector<Barco> barcosIA = {
      // Barco('P', 5),
      // Barco('B', 4),
      // Barco('B', 4),
      // Barco('S', 3),
      Barco('S', 3),
      Barco('L', 1),
      Barco('L', 1),
      Barco('L', 1)};

  tableroJugador.colocarBarcos(barcosJugador);
  tableroIA.colocarBarcos(barcosIA);

  while (!tableroJugador.todosHundidos() && !tableroIA.todosHundidos())
  {

    std::cout << "\n\tTABLERO MIS EMBARCACIONES" << std::endl;
    tableroJugador.imprimirTableroConBarcos();

    std::cout << "\n\tTABLERO DISPAROS" << std::endl;
    tableroJugador.imprimirTablero();

    std::cout << "\n\tTABLERO IA" << std::endl;
    tableroIA.imprimirTableroConBarcos();

    std::string entrada;
    int fila, columna;

    do
    {
      std::cout << "Introduzca las coordenadas de su ataque (letra numero): ";
      std::cin >> entrada;
    } while (!obtenerCoordenadas(entrada, fila, columna));

    tableroIA.ataque(fila, columna);

    if (tableroIA.todosHundidos())
    {
      std::cout << "Has ganado\n"
                << std::endl;
      break;
    }

    std::cout << "Turno de la IA...\n"
              << std::endl;
    moverIA(tableroJugador);

    if (tableroJugador.todosHundidos())
    {
      std::cout << "La IA ha ganado\n"
                << std::endl;
      break;
    }
  }

  std::cout << "Fin del juego\n";

  return 0;
}
