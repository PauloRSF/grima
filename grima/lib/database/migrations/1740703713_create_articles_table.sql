CREATE TABLE articles (
  id UUID PRIMARY KEY,
  author_id UUID,
  slug VARCHAR(128) NOT NULL,
  title VARCHAR(128) NOT NULL,
  description VARCHAR(255) NOT NULL,
  body TEXT,
  created_at TIMESTAMP NOT NULL,
  updated_at TIMESTAMP NOT NULL,
  CONSTRAINT article_unique_slug UNIQUE (slug),
  CONSTRAINT article_has_author FOREIGN KEY (author_id) REFERENCES authors (id)
);
