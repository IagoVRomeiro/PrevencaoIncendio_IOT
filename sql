-- ## 1. CONSULTAS BÁSICAS ##
-- Seleciona todos os registros de uma tabela
SELECT * FROM tabela;

-- Seleciona colunas específicas
SELECT coluna1, coluna2 FROM tabela;

-- Renomeia uma coluna no resultado da consulta
SELECT coluna1 AS alias FROM tabela;

-- Filtra registros com uma condição
SELECT * FROM tabela WHERE coluna = 'valor';

-- Ordena registros em ordem crescente ou decrescente
SELECT * FROM tabela ORDER BY coluna ASC;  -- Crescente
SELECT * FROM tabela ORDER BY coluna DESC; -- Decrescente

-- Limita o número de registros retornados
SELECT * FROM tabela LIMIT 10;

-- ## 2. FILTROS AVANÇADOS ##
-- Filtra usando operadores lógicos (AND e OR)
SELECT * FROM tabela WHERE coluna1 = 'valor' AND coluna2 > 10;
SELECT * FROM tabela WHERE coluna1 = 'valor' OR coluna2 < 5;

-- Filtra por intervalo de valores
SELECT * FROM tabela WHERE coluna BETWEEN 10 AND 20;

-- Filtra por lista de valores
SELECT * FROM tabela WHERE coluna IN ('valor1', 'valor2');

-- Filtra por padrões de texto (uso de LIKE)
SELECT * FROM tabela WHERE coluna LIKE 'teste%';  -- Começa com 'teste'
SELECT * FROM tabela WHERE coluna LIKE '%teste';  -- Termina com 'teste'
SELECT * FROM tabela WHERE coluna LIKE '%teste%'; -- Contém 'teste'

-- ## 3. FUNÇÕES AGREGADAS ##
-- Conta o número de registros
SELECT COUNT(*) FROM tabela;

-- Calcula soma, média, máximo e mínimo
SELECT SUM(coluna) FROM tabela;
SELECT AVG(coluna) FROM tabela;
SELECT MAX(coluna) FROM tabela;
SELECT MIN(coluna) FROM tabela;

-- Agrupa registros e conta ocorrências
SELECT coluna, COUNT(*) FROM tabela GROUP BY coluna;

-- Filtra os grupos gerados
SELECT coluna, COUNT(*) FROM tabela GROUP BY coluna HAVING COUNT(*) > 5;

-- ## 4. JUNÇÕES (JOINS) ##
-- Inner Join: apenas registros correspondentes
SELECT * 
FROM tabela1
INNER JOIN tabela2 ON tabela1.coluna = tabela2.coluna;

-- Left Join: todos da tabela1 e correspondentes da tabela2
SELECT * 
FROM tabela1
LEFT JOIN tabela2 ON tabela1.coluna = tabela2.coluna;

-- Right Join: todos da tabela2 e correspondentes da tabela1
SELECT * 
FROM tabela1
RIGHT JOIN tabela2 ON tabela1.coluna = tabela2.coluna;

-- ## 5. SUBCONSULTAS ##
-- Consulta dentro de outra consulta
SELECT * FROM tabela WHERE coluna IN (SELECT coluna FROM outra_tabela);

-- União de resultados de consultas
SELECT coluna FROM tabela1
UNION
SELECT coluna FROM tabela2;

-- ## 6. ALTERAÇÃO DE DADOS ##
-- Atualiza registros existentes
UPDATE tabela SET coluna = 'novo_valor' WHERE coluna = 'valor';

-- Exclui registros específicos
DELETE FROM tabela WHERE coluna = 'valor';

-- Apaga todos os dados da tabela (sem reiniciar IDs)
DELETE FROM tabela;

-- Apaga todos os dados da tabela e reinicia IDs
TRUNCATE TABLE tabela;

-- ## 7. VISUALIZAR ESTRUTURA DO BANCO ##
-- Lista todos os bancos de dados
SHOW DATABASES;

-- Seleciona um banco de dados para uso
USE nome_banco;

-- Lista todas as tabelas no banco
SHOW TABLES;

-- Exibe a estrutura de uma tabela
DESCRIBE tabela;

-- Exibe o comando SQL usado para criar uma tabela
SHOW CREATE TABLE tabela;

-- ## 8. COMANDOS PARA TESTES ##
-- Criar uma tabela de exemplo, se não existir
CREATE TABLE IF NOT EXISTS leituras (
    id INT AUTO_INCREMENT PRIMARY KEY,
    topico VARCHAR(255),
    valor VARCHAR(255),
    data_hora TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Inserir dados de exemplo
INSERT INTO leituras (topico, valor) VALUES ('topico_teste', 'valor_teste');

-- Consultar os dados inseridos
SELECT * FROM leituras;

-- Limpar a tabela após os testes
TRUNCATE TABLE leituras;
