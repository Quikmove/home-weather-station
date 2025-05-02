import { ANON_KEY, DATABASE_URL  } from '$env/static/private';
import { createClient } from '@supabase/supabase-js'
 
export const supabase = createClient(DATABASE_URL, ANON_KEY);