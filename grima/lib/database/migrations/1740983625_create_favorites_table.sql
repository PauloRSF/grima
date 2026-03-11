CREATE TABLE favorites (
  id UUID PRIMARY KEY,
  author_id UUID,
  article_id UUID,
  created_at TIMESTAMP,
  CONSTRAINT favorite_belongs_to_author FOREIGN KEY (author_id) REFERENCES authors (id),
  CONSTRAINT favorite_belongs_to_article FOREIGN KEY (article_id) REFERENCES articles (id)
);
