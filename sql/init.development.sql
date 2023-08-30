CREATE DATABASE grima_development;

\c grima_development;

CREATE TABLE IF NOT EXISTS persons  (
  id UUID PRIMARY KEY,
  name VARCHAR(100) NOT NULL,
  nickname VARCHAR(32) NOT NULL UNIQUE,
  date_of_birth DATE NOT NULL
);

CREATE TABLE IF NOT EXISTS person_techs  (
  person_id UUID NOT NULL,
  name VARCHAR(32) NOT NULL,

  FOREIGN KEY (person_id) REFERENCES persons(id) ON DELETE CASCADE
);

CREATE INDEX person_techs_person_id_i ON person_techs(person_id);
