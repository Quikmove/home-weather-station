import { DATABASE_URL, ANON_KEY } from '$env/static/private'
import { createClient } from '@supabase/supabase-js'
 
export const supabase = createClient(DATABASE_URL, ANON_KEY)