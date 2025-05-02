import { DATABASE_URL, SUPABASE_SERVICE_ROLE_KEY,  } from '$env/static/private';
import { createClient } from '@supabase/supabase-js'
 
export const supabase = createClient(DATABASE_URL, SUPABASE_SERVICE_ROLE_KEY);