CREATE TABLE sessions (
  id VARCHAR(64) PRIMARY KEY,
  account_id UUID NOT NULL,
  created_at TIMESTAMP NOT NULL,
  updated_at TIMESTAMP NOT NULL,
  FOREIGN KEY (account_id) REFERENCES accounts (id)
);
