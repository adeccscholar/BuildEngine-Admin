-- BuildEngine migration 4: align persisted state IDs with LibraryStateOrder.
-- The scope table is a materialized FSM cache. Entries for 80/90 were written
-- under incompatible state numbering and cannot be interpreted reliably.
-- Filesystem state evidence remains authoritative; next reconciliation refills
-- the invalidated cache rows.
BEGIN;

DO $migration$
BEGIN
   IF NOT EXISTS (
      SELECT 1 FROM information_schema.tables
      WHERE table_schema='buildengine' AND table_name='dom_library_state'
   ) OR NOT EXISTS (
      SELECT 1 FROM information_schema.tables
      WHERE table_schema='buildengine' AND table_name='library_scope_state'
   ) THEN
      RAISE EXCEPTION 'BuildEngine base FSM tables are missing; apply baseline/Update_1 before Update_4';
   END IF;
END
$migration$;

DELETE FROM buildengine.library_scope_state
WHERE state_id IN (80,90,100);

-- The old IDs represented documentation=80, ready=90. Drop those domain
-- records only after clearing the derived cache to honor the foreign key.
DELETE FROM buildengine.dom_library_state
WHERE state_id IN (80,90,100);

INSERT INTO buildengine.dom_library_state
   (state_id,state_code,description,notice)
VALUES
   (80,'smoke','Consumer smoke scope completed',''),
   (90,'documentation','Documentation scope completed','May contain dynamically materialized scopes.'),
   (100,'ready','Logical library ready state','Derived from the FSM.')
ON CONFLICT (state_id) DO UPDATE
SET state_code=EXCLUDED.state_code,
    description=EXCLUDED.description,
    notice=EXCLUDED.notice;

DO $verify$
BEGIN
   IF EXISTS (
      SELECT 1 FROM (VALUES
         (10,'source'),(20,'build'),(30,'test'),(40,'validation'),
         (50,'install'),(60,'metadata'),(70,'publish'),(80,'smoke'),
         (90,'documentation'),(100,'ready')
      ) AS expected(state_id,state_code)
      LEFT JOIN buildengine.dom_library_state AS actual
         ON actual.state_id=expected.state_id
        AND actual.state_code=expected.state_code
      WHERE actual.state_id IS NULL
   ) THEN
      RAISE EXCEPTION 'BuildEngine state domain does not match the 10-state FSM';
   END IF;
END
$verify$;

COMMIT;
