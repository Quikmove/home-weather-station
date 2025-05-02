import { SECRET_VALUE, SUPABASE_ACCESS_TOKEN, SUPABASE_SERVER_ROLE_KEY } from "$env/static/private";
import { supabase } from "$lib/supabaseClient";
import { createClient } from "@supabase/supabase-js";

import { json } from "@sveltejs/kit";

type WeatherData = {
    temperature: number;
    humidity: number;
};
export async function POST({ request }: { request: Request }) {
    const privateSupabase = createClient(SUPABASE_ACCESS_TOKEN, SUPABASE_SERVER_ROLE_KEY);
    const data = await request.json();
    // if (secretHeader !== "gicji6-wovcAp-nunbad") {
    //     return json("Unauthorized", { status: 401 });
    // }
    const { temperature, humidity } = data;
    if (
        typeof temperature !== "number" ||
        typeof humidity !== "number"
    ) {
        return  json("Invalid data", { status: 400 });
    }
    const res = await privateSupabase.from("HomeData").insert(
        {
            temperature: temperature,
            humidity: humidity,
        }
    ).select();
    if(res.error) return json("error", { status: 201 });
    else return json(res.data, { status: 201 });
}
export async function GET() {
    // select recent record
    const { data, error } = await supabase.functions.invoke('get-last-query', {
       body: { name: 'Functions'}
      })
    if (error) {
        return json({error: error.message}, { status: 404 });
    }
    return json(data, {
        status: 200,
        headers: { "Content-Type": "application/json" },
    });
}