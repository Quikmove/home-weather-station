import { sql } from 'drizzle-orm';
import { sqliteTable, text, integer, real, SQLiteTimestamp } from 'drizzle-orm/sqlite-core';

// export const user = sqliteTable('user', {
// 	id: integer('id').primaryKey(),
// 	age: integer('age')
// });
export const home_data = sqliteTable('home_data', {
	id: integer('id').primaryKey(),
	temperature: real('temperature').notNull(),
	humidity: real('humidity').notNull(),
	created: text("created").default(sql`(CURRENT_TIMESTAMP)`),
});
