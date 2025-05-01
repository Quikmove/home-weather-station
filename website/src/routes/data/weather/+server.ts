import { SECRET_VALUE } from "$env/static/private";
import { supabase } from "$lib/supabaseClient";

import { json } from "@sveltejs/kit";

type WeatherData = {
    temperature: number;
    humidity: number;
};
export async function POST({ request }: { request: Request }) {
    const data = await request.json();
    const secretHeader = request.headers.get("x-secret-header");
    if (secretHeader !== SECRET_VALUE) {
        return json("Unauthorized", { status: 401 });
    }
    const { temperature, humidity } = data;
    if (
        typeof temperature !== "number" ||
        typeof humidity !== "number"
    ) {
        return  json("Invalid data", { status: 400 });
    }
    const res = await supabase.from("HomeData").insert(
        {
            temperature: temperature,
            humidity: humidity,
        }
    ).select();
    if(res.error) return json("error", { status: 201 });
    else return json(res.data, { status: 201 });
}
export async function GET() {
    // select 3 most recent records
    const { data } = await supabase
        .from("HomeData")
        .select("temperature, humidity, created_at")
        .order("created_at", { ascending: false })
        .limit(1);
    // if (!data) {
    //     return json("No data found", { status: 404 });
    // }
    return json(data, {
        status: 200,
        headers: { "Content-Type": "application/json" },
    });
}