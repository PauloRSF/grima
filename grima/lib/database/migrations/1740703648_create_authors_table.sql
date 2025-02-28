CREATE TABLE authors (
  id UUID PRIMARY KEY,
  username VARCHAR(255) NOT NULL,
  bio TEXT,
  image VARCHAR(255),
  created_at TIMESTAMP NOT NULL,
  updated_at TIMESTAMP NOT NULL,
  CONSTRAINT author_unique_username UNIQUE (username)
);
