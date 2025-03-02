CREATE TABLE comments (
  id UUID PRIMARY KEY,
  author_id UUID,
  article_id UUID,
  body TEXT,
  created_at TIMESTAMP NOT NULL,
  updated_at TIMESTAMP NOT NULL,
  CONSTRAINT comment_has_author FOREIGN KEY (author_id) REFERENCES authors (id),
  CONSTRAINT comment_has_article FOREIGN KEY (article_id) REFERENCES articles (id)
);
