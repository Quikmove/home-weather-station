import { sqliteTable, AnySQLiteColumn, integer, real, text } from "drizzle-orm/sqlite-core"
  import { sql } from "drizzle-orm"

export const homeData = sqliteTable("home_data", {
	id: integer().primaryKey().notNull(),
	temperature: real().notNull(),
	humidity: real().notNull(),
	created: text().default("sql`(CURRENT_TIMESTAMP)`"),
});

