<script lang="ts">
import * as Card from "$lib/components/ui/card/";
export let data;
console.log(data);

</script>
<div class="card-container">
    <Card.Root>
            <Card.Content class="p-4">
                <h2>Temperature</h2>
                <p>{data.homeData.temperature}°C</p>
            </Card.Content>
            <Card.Content>
                <h2>Humidity</h2>
                <p>{data.homeData.humidity}%</p>
            </Card.Content>
            <Card.Footer>
                <p>Logged at: {new Date(data.homeData.created_at).toLocaleString()}</p>
            </Card.Footer>
        </Card.Root>
    </div>

<style>
    .card-container {
        display: flex;
        justify-content: center;
        align-items: center;
        margin: 1rem;
    }
</style>