# Algoritmo do Banqueiro

Implementação em C do algoritmo do banqueiro, que controla a alocação de
recursos entre clientes e evita concessões que deixem o sistema em um
estado inseguro.

O programa lê os recursos disponíveis pela linha de comando, os limites
dos clientes em `customer.txt` e os comandos em `commands.txt`. Os
resultados são gravados em `result.txt`.

## Compilação

No terminal, dentro da pasta do projeto, execute:

```bash
make
```

O comando cria o executável `banker`.

## Arquivo de clientes

O arquivo `customer.txt` deve estar na pasta de execução. Cada linha
representa um cliente, e cada valor indica o máximo que ele pode
solicitar de um tipo de recurso:

```text
7,5,3
3,2,2
9,0,2
2,2,2
4,3,3
```

Os clientes são numerados a partir de zero, na ordem das linhas. Todas
as linhas devem ter a mesma quantidade de valores que os recursos
informados na execução.

## Arquivo de comandos

O arquivo `commands.txt` contém um comando por linha:

```text
RQ 0 0 2 1
RL 0 0 1 1
*
```

- `RQ` solicita recursos. Após o comando vêm o número do cliente e as
  quantidades solicitadas.
- `RL` libera recursos que o cliente possui. O formato é o mesmo de
  `RQ`.
- `*` registra o estado atual das matrizes e dos recursos disponíveis.

Uma solicitação é negada se ultrapassar a necessidade do cliente, se
não houver recursos suficientes ou se a concessão deixar o sistema em
um estado inseguro.

## Execução

Informe como argumentos as quantidades disponíveis de cada recurso:

```bash
./banker 10 5 7
```

Nesse exemplo, o sistema começa com três tipos de recursos, com 10, 5 e
7 instâncias disponíveis. Após a execução, consulte o resultado com:

```bash
cat result.txt
```

## Limpeza

Para remover o executável gerado pela compilação:

```bash
make clean
```