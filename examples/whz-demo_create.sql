-- Drop the table if it already exists
DROP TABLE IF EXISTS contact_addresses;

-- Create a new SQLite database and a table for contact addresses
CREATE TABLE contact_addresses (
                                   id INTEGER PRIMARY KEY AUTOINCREMENT,
                                   first_name TEXT NOT NULL,
                                   last_name TEXT NOT NULL,
                                   email TEXT NOT NULL,
                                   phone TEXT,
                                   street TEXT,
                                   city TEXT,
                                   state TEXT,
                                   postal_code TEXT,
                                   country TEXT
);

-- Insert 100 address records into the table
INSERT INTO contact_addresses (first_name, last_name, email, phone, street, city, state, postal_code, country)
VALUES
    ('John', 'Doe', 'john.doe@example.com', '123-456-7890', '123 Main St', 'Zürich', 'ZH', '12345', 'Switzerland'),
    ('Jane', 'Smith', 'jane.smith@example.com', '234-567-8901', '456 Elm St', 'Winterthur', 'ZH', '23456', 'Switzerland'),
-- Add 98 more records here
    ('FirstName100', 'LastName100', 'email100@example.com', '100-000-0000', '100 Address St', 'Berlin', 'Brandenburg', '10000', 'Germany');