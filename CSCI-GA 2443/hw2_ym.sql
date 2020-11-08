-- Database Systems, CSCI-GA 2433-001, NYU, Fall 2020, Homework Asssignment #2

CREATE TABLE employee  (
    name VARCHAR(255) NOT NULL,
    salary INT NOT NULL,
    manager VARCHAR(255),
    department VARCHAR(255) NOT NULL,
    PRIMARY KEY (name)
);

CREATE TABLE course(
    student VARCHAR(255) NOT NULL,
    subj VARCHAR(255) NOT NULL,
    prof VARCHAR(255) NOT NULL,
    grade INT NOT NULL,
    PRIMARY KEY (student, subj)
);

INSERT INTO employee VALUES ('Smith', 31000, 'Jones', 'Switch');
INSERT INTO employee VALUES ('Patten', 28000, NULL, 'Software');
INSERT INTO employee VALUES ('Hughes', 33000, 'Jones', 'Switch');
INSERT INTO employee VALUES ('Jones', 32000, 'Patten', 'Switch');
INSERT INTO employee VALUES ('Warren', 40000, 'Patten', 'Software');

INSERT INTO course VALUES ('Smith', 'Algs', 'Hackett', 85);
INSERT INTO course VALUES ('Patten', 'Algs', 'Hackett', 80);
INSERT INTO course VALUES ('Patten', 'Comp', 'Roe', 70);
INSERT INTO course VALUES ('Jones', 'Comp', 'Roe', 75);
INSERT INTO course VALUES ('Jones', 'Dbase', 'Black', 80);
INSERT INTO course VALUES ('Warren', 'Dbase', 'Black', 75);
INSERT INTO course VALUES ('Warren', 'Comp', 'Roe', 65);

-- Query 1 --
SELECT name 
FROM employee
WHERE  department = 'Software';

-- Query 2 --
SELECT e.name 
FROM employee AS e, employee AS m
WHERE e.manager = m.name AND e.salary >= m.salary + 5000;

-- Query 3 --
SELECT e.name
FROM employee AS e, course AS ce, employee AS m, course AS cm
WHERE e.name = ce.student AND m.name = cm.student AND e.manager = m.name AND ce.subj = cm.subj AND ce.grade > cm.grade;

-- Query 4 --
SELECT name
FROM employee
WHERE name NOT IN (
SELECT DISTINCT student
FROM course
);
# Equivalently
SELECT name
FROM employee
WHERE NOT EXISTS (
SELECT DISTINCT student
FROM course
WHERE name = student
);

-- Query 5 --
SELECT DISTINCT department 
FROM employee
WHERE department NOT IN (
SELECT department
FROM employee
WHERE name IN (
SELECT name
FROM employee
WHERE name NOT IN (
SELECT DISTINCT student
FROM course
)));
# Equivalently
SELECT DISTINCT e1.department
FROM employee e1
WHERE NOT EXISTS(
SELECT * 
FROM employee e2
WHERE e1.department = e2.department AND EXISTS(
SELECT * 
FROM employee e3
WHERE e2.name = e3.name AND NOT EXISTS(
SELECT * 
FROM course
WHERE student = e3.name
)));

-- Query 6 --
SELECT DISTINCT department 
FROM employee
WHERE department NOT IN (
SELECT department
FROM employee
WHERE name IN (
SELECT name
FROM employee
WHERE name NOT IN(
SELECT name
FROM employee LEFT JOIN course ON name = student
GROUP BY name
HAVING count(*) >= 2
)));

-- Query 7 --
SELECT AVG(salary)
FROM employee;

-- Query 8--
SELECT MIN(salary)
FROM employee, course
WHERE name = student AND prof = 'Roe';

-- Query 9--
SELECT AVG(salary)
FROM employee
WHERE department IN (
SELECT department
FROM employee, course
WHERE name = student
GROUP BY department
HAVING count(*) > 3)
GROUP BY department;

-- Query 10--
SELECT department, (1 - (count(DISTINCT student) / count(DISTINCT name))) * 100
FROM employee LEFT JOIN course ON name = student
GROUP BY department;

DROP TABLE employee;
DROP TABLE course;